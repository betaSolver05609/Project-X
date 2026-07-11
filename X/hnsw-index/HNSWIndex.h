#pragma once
#include <vector>
#include <utility>
#include <functional>
#include <random>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <limits>
#include "../similarity-engines/similarity.h"

// A from-scratch HNSW (Hierarchical Navigable Small World) approximate nearest-neighbor
// graph. Nodes are addressed by an external integer id (in practice, a record's index in
// Block::records) rather than owning any vector storage themselves — callers pass an
// EmbeddingAccessor + SimilarityEngine into every call. That keeps this class decoupled from
// Block::Record (no circular include with block/Block.h) and means it holds nothing but plain
// ints/vectors, so it's trivially and correctly deep-copyable via the compiler-generated copy
// constructor — important since Block (and therefore this) gets deep-copied on every
// TransactionContext snapshot.
//
// Records are never deleted individually in this codebase (only whole keyspaces are dropped),
// so node ids never need to be reused or tombstoned.
class HNSWIndex
{
public:
    using EmbeddingAccessor = std::function<const std::vector<float> &(int)>;

    void insert(int id, const EmbeddingAccessor &getEmbedding, const SimilarityEngine &engine)
    {
        const std::vector<float> &vec = getEmbedding(id);

        int level = randomLevel();
        if (static_cast<size_t>(id) >= nodes.size())
            nodes.resize(id + 1);
        nodes[id].level = level;
        nodes[id].neighbors.assign(level + 1, {});

        if (entryPoint == -1)
        {
            entryPoint = id;
            topLevel = level;
            return;
        }

        int ep = entryPoint;

        // Greedily descend from the top layer to level+1 (ef=1) to find a good entry point
        // for the layers this node actually participates in.
        for (int lc = topLevel; lc > level; lc--)
            ep = greedyClosest(vec, ep, lc, getEmbedding, engine);

        for (int lc = std::min(level, topLevel); lc >= 0; lc--)
        {
            auto candidates = searchLayer(vec, ep, EF_CONSTRUCTION, lc, getEmbedding, engine);
            auto neighbors = selectNeighbors(candidates, maxNeighbors(lc));

            for (auto &entry : neighbors)
            {
                int neighborId = entry.second;
                connect(id, neighborId, lc);
                connect(neighborId, id, lc);
                pruneIfNeeded(neighborId, lc, getEmbedding, engine);
            }

            if (!candidates.empty())
                ep = candidates.front().second; // closest found at this layer feeds the next
        }

        if (level > topLevel)
        {
            topLevel = level;
            entryPoint = id;
        }
    }

    // Returns up to k results, sorted descending by similarity.
    std::vector<std::pair<int, float>> search(const std::vector<float> &query, int k, int ef,
                                                const EmbeddingAccessor &getEmbedding,
                                                const SimilarityEngine &engine) const
    {
        if (entryPoint == -1)
            return {};

        int ep = entryPoint;
        for (int lc = topLevel; lc > 0; lc--)
            ep = greedyClosest(query, ep, lc, getEmbedding, engine);

        auto candidates = searchLayer(query, ep, std::max(ef, k), 0, getEmbedding, engine);

        std::vector<std::pair<int, float>> results;
        results.reserve(std::min<size_t>(candidates.size(), static_cast<size_t>(k)));
        for (size_t i = 0; i < candidates.size() && static_cast<int>(i) < k; i++)
            results.emplace_back(candidates[i].second, candidates[i].first);

        return results;
    }

    void clear()
    {
        nodes.clear();
        entryPoint = -1;
        topLevel = -1;
    }

    bool empty() const { return entryPoint == -1; }

private:
    struct Node
    {
        int level = 0;
        std::vector<std::vector<int>> neighbors; // neighbors[layer] = neighbor ids at that layer
    };

    std::vector<Node> nodes;
    int entryPoint = -1;
    int topLevel = -1;

    static constexpr int M = 16;
    static constexpr int M_MAX0 = 32;
    static constexpr int EF_CONSTRUCTION = 200;

    int maxNeighbors(int layer) const { return layer == 0 ? M_MAX0 : M; }

    int randomLevel() const
    {
        static thread_local std::mt19937 rng{std::random_device{}()};
        static thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
        double mL = 1.0 / std::log(static_cast<double>(M));
        double r = dist(rng);
        if (r <= 0.0)
            r = 1e-9; // guard against log(0)
        return static_cast<int>(std::floor(-std::log(r) * mL));
    }

    // A dimension mismatch (allowed — keyspaces have no fixed schema) makes a node
    // unreachable via this comparison instead of crashing graph construction/search.
    float safeCompute(const std::vector<float> &a, const std::vector<float> &b,
                       const SimilarityEngine &engine) const
    {
        try
        {
            return engine.compute(a, b);
        }
        catch (const std::exception &)
        {
            return -std::numeric_limits<float>::infinity();
        }
    }

    int greedyClosest(const std::vector<float> &query, int ep, int layer,
                       const EmbeddingAccessor &getEmbedding, const SimilarityEngine &engine) const
    {
        int current = ep;
        float currentSim = safeCompute(query, getEmbedding(current), engine);

        bool improved = true;
        while (improved)
        {
            improved = false;
            if (static_cast<size_t>(current) >= nodes.size())
                break;
            if (layer >= static_cast<int>(nodes[current].neighbors.size()))
                break;

            for (int neighborId : nodes[current].neighbors[layer])
            {
                float sim = safeCompute(query, getEmbedding(neighborId), engine);
                if (sim > currentSim)
                {
                    currentSim = sim;
                    current = neighborId;
                    improved = true;
                }
            }
        }
        return current;
    }

    // Returns candidates sorted descending by similarity (best first), capped at ef.
    std::vector<std::pair<float, int>> searchLayer(const std::vector<float> &query, int ep, int ef,
                                                     int layer, const EmbeddingAccessor &getEmbedding,
                                                     const SimilarityEngine &engine) const
    {
        auto cmpMax = [](const std::pair<float, int> &a, const std::pair<float, int> &b)
        { return a.first < b.first; }; // max-heap: highest similarity explored first
        auto cmpMin = [](const std::pair<float, int> &a, const std::pair<float, int> &b)
        { return a.first > b.first; }; // min-heap: lowest similarity evicted first

        std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, decltype(cmpMax)>
            candidates(cmpMax);
        std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, decltype(cmpMin)>
            found(cmpMin);
        std::unordered_set<int> visited;

        float epSim = safeCompute(query, getEmbedding(ep), engine);
        candidates.push({epSim, ep});
        found.push({epSim, ep});
        visited.insert(ep);

        while (!candidates.empty())
        {
            std::pair<float, int> curr = candidates.top();
            candidates.pop();
            float simC = curr.first;
            int c = curr.second;

            if (static_cast<int>(found.size()) >= ef && simC < found.top().first)
                break;

            if (static_cast<size_t>(c) >= nodes.size() || layer >= static_cast<int>(nodes[c].neighbors.size()))
                continue;

            for (int e : nodes[c].neighbors[layer])
            {
                if (visited.count(e))
                    continue;
                visited.insert(e);

                float simE = safeCompute(query, getEmbedding(e), engine);
                if (static_cast<int>(found.size()) < ef || simE > found.top().first)
                {
                    candidates.push({simE, e});
                    found.push({simE, e});
                    if (static_cast<int>(found.size()) > ef)
                        found.pop();
                }
            }
        }

        std::vector<std::pair<float, int>> result;
        result.reserve(found.size());
        while (!found.empty())
        {
            result.push_back(found.top());
            found.pop();
        }
        std::sort(result.begin(), result.end(),
                  [](const std::pair<float, int> &a, const std::pair<float, int> &b)
                  { return a.first > b.first; });
        return result;
    }

    std::vector<std::pair<float, int>> selectNeighbors(const std::vector<std::pair<float, int>> &candidates,
                                                         int m) const
    {
        if (static_cast<int>(candidates.size()) <= m)
            return candidates;
        return std::vector<std::pair<float, int>>(candidates.begin(), candidates.begin() + m);
    }

    void connect(int from, int to, int layer)
    {
        if (static_cast<size_t>(from) >= nodes.size())
            return;
        if (layer >= static_cast<int>(nodes[from].neighbors.size()))
            nodes[from].neighbors.resize(layer + 1);

        auto &lst = nodes[from].neighbors[layer];
        if (std::find(lst.begin(), lst.end(), to) == lst.end())
            lst.push_back(to);
    }

    void pruneIfNeeded(int nodeId, int layer, const EmbeddingAccessor &getEmbedding,
                        const SimilarityEngine &engine)
    {
        if (static_cast<size_t>(nodeId) >= nodes.size())
            return;
        if (layer >= static_cast<int>(nodes[nodeId].neighbors.size()))
            return;

        auto &lst = nodes[nodeId].neighbors[layer];
        int limit = maxNeighbors(layer);
        if (static_cast<int>(lst.size()) <= limit)
            return;

        const std::vector<float> &selfVec = getEmbedding(nodeId);
        std::vector<std::pair<float, int>> scored;
        scored.reserve(lst.size());
        for (int n : lst)
            scored.push_back({safeCompute(selfVec, getEmbedding(n), engine), n});

        std::sort(scored.begin(), scored.end(),
                  [](const std::pair<float, int> &a, const std::pair<float, int> &b)
                  { return a.first > b.first; });
        scored.resize(limit);

        lst.clear();
        for (auto &entry : scored)
            lst.push_back(entry.second);
    }
};
