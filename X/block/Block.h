#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <memory>
#include <stdexcept>
#include "common_utils/json_portable.h"
#include "similarity-engines/factory.h"
#include "hnsw-index/HNSWIndex.h"

class Block
{
public:
    struct Record
    {
        std::vector<float> embedding;
        std::string text; // empty if the record was inserted as a raw vector
    };

private:
    std::vector<Record> records;
    std::string metric;
    std::unique_ptr<SimilarityEngine> engine;
    HNSWIndex hnsw;

    // Below this size, an exact O(n) brute-force scan is fast enough that there's no reason
    // to trust the approximate index instead. Also gates Mahalanobis out entirely — it needs
    // whole-dataset context (computeWithDataset), which doesn't fit a pairwise graph index,
    // and is the one metric no code path here ever actually initializes for real use anyway.
    static constexpr size_t HNSW_THRESHOLD = 500;

    bool hnswEligible() const { return metric != "MAHALABONIS"; }

    HNSWIndex::EmbeddingAccessor embeddingAccessor() const
    {
        return [this](int i) -> const std::vector<float> & { return records[i].embedding; };
    }

    // Rebuilds the graph from scratch against the current metric. Needed whenever the
    // similarity function changes (ALTER), since a graph built for one metric's notion of
    // "close" doesn't necessarily reflect another's.
    void rebuildIndex()
    {
        hnsw.clear();
        if (!hnswEligible())
            return;

        auto accessor = embeddingAccessor();
        for (size_t i = 0; i < records.size(); i++)
        {
            try
            {
                hnsw.insert(static_cast<int>(i), accessor, *engine);
            }
            catch (const std::exception &)
            {
                // Leave this record out of the graph; it stays fully visible via the
                // brute-force fallback path regardless.
            }
        }
    }

    std::vector<std::pair<int, float>> bruteForceSearch(const std::vector<float> &query, int top_k) const
    {
        std::vector<std::pair<int, float>> sims;

        std::vector<std::vector<float>> embeddingsOnly;
        if (metric == "MAHALABONIS")
        {
            embeddingsOnly.reserve(records.size());
            for (auto &r : records)
                embeddingsOnly.push_back(r.embedding);
        }

        for (size_t i = 0; i < records.size(); i++)
        {
            // A record whose dimension doesn't match the query (allowed, since keyspaces
            // have no fixed schema) is simply excluded from results rather than crashing
            // the whole search — engines signal this via std::invalid_argument.
            try
            {
                float sim;
                if (metric == "MAHALABONIS")
                {
                    sim = engine->computeWithDataset(query, records[i].embedding, embeddingsOnly);
                }
                else
                {
                    sim = engine->compute(query, records[i].embedding);
                }
                sims.emplace_back(i, sim);
            }
            catch (const std::exception &)
            {
                continue;
            }
        }

        std::sort(sims.begin(), sims.end(),
                  [](auto &a, auto &b)
                  { return a.second > b.second; });

        if ((int)sims.size() > top_k)
            sims.resize(top_k);

        return sims;
    }

public:
    // Constructor
    Block(const std::string &metricType = "cosine")
        : metric(metricType), engine(createSimilarityEngine(metricType)) {}

    // Deep copy constructor
    Block(const Block &other)
        : records(other.records), metric(other.metric),
          engine(createSimilarityEngine(other.metric)), hnsw(other.hnsw) {}

    // Deep copy assignment operator
    Block &operator=(const Block &other)
    {
        if (this != &other)
        {
            records = other.records;
            metric = other.metric;
            engine = createSimilarityEngine(other.metric); // re-create engine
            hnsw = other.hnsw;
        }
        return *this;
    }

    // Move constructor and assignment (default works fine)
    Block(Block &&) noexcept = default;
    Block &operator=(Block &&) noexcept = default;

    // Metric functions
    void setMetric(const std::string &m)
    {
        metric = m;
        engine = createSimilarityEngine(m);
        rebuildIndex();
    }

    std::string getMetric() const { return metric; }

    // Record management
    void insertRecord(const std::vector<float> &record, const std::string &text = "")
    {
        records.push_back({record, text});

        if (hnswEligible())
        {
            try
            {
                hnsw.insert(static_cast<int>(records.size()) - 1, embeddingAccessor(), *engine);
            }
            catch (const std::exception &)
            {
                // The record is safely stored in `records` either way; if graph maintenance
                // failed, this keyspace just falls back to brute-force visibility.
            }
        }
    }

    std::vector<std::vector<float>> getRecords() const
    {
        std::vector<std::vector<float>> out;
        out.reserve(records.size());
        for (auto &r : records)
            out.push_back(r.embedding);
        return out;
    }

    const std::vector<Record> &getRecordsFull() const
    {
        return records;
    }

    std::string getRecordText(int index) const
    {
        if (index >= 0 && static_cast<size_t>(index) < records.size())
            return records[index].text;
        return "";
    }

    std::vector<std::pair<int, std::vector<float>>> getRecordsWithId() const
    {
        std::vector<std::pair<int, std::vector<float>>> out;
        for (size_t i = 0; i < records.size(); i++)
            out.push_back({(int)i, records[i].embedding});
        return out;
    }

    // Nearest neighbor search. Uses the HNSW graph once the keyspace is large enough for an
    // approximate index to be worth it; otherwise (or on any trouble) falls back to the exact
    // brute-force scan, which remains fully correct and is what small/typical keyspaces use.
    std::vector<std::pair<int, float>> findNearest(const std::vector<float> &query, int top_k = 1) const
    {
        if (hnswEligible() && records.size() > HNSW_THRESHOLD && !hnsw.empty())
        {
            try
            {
                int ef = std::max(2 * top_k, 50);
                auto results = hnsw.search(query, top_k, ef, embeddingAccessor(), *engine);
                if (!results.empty())
                    return results;
            }
            catch (const std::exception &)
            {
                // Fall through to the proven brute-force path below.
            }
        }

        return bruteForceSearch(query, top_k);
    }

    // JSON serialization
    Json::Value toJson() const
    {
        Json::Value root;
        root["metric"] = metric;
        for (auto &rec : records)
        {
            Json::Value arr(Json::arrayValue);
            for (auto v : rec.embedding)
                arr.append(v);
            Json::Value entry;
            entry["embedding"] = arr;
            entry["text"] = rec.text;
            root["records"].append(entry);
        }
        return root;
    }

    void fromJson(const Json::Value &root)
    {
        metric = root.get("metric", "cosine").asString();
        engine = createSimilarityEngine(metric);
        records.clear();
        for (auto &rec : root["records"])
        {
            std::vector<float> vec;
            for (auto &v : rec["embedding"])
                vec.push_back(v.asFloat());
            records.push_back({vec, rec.get("text", "").asString()});
        }
        rebuildIndex();
    }
};
