#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <jsoncpp/json/json.h> // ✅ required for persistence

class Block
{
private:
    std::vector<std::vector<float>> records;
    std::string metric; // similarity metric: cosine, euclidean, dot

    // --- helper functions ---
    float cosineSimilarity(const std::vector<float> &a, const std::vector<float> &b) const
    {
        float dot = 0.0, normA = 0.0, normB = 0.0;
        for (size_t i = 0; i < a.size(); i++)
        {
            dot += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        if (normA == 0 || normB == 0)
            return 0.0f;
        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }

    float euclideanDistance(const std::vector<float> &a, const std::vector<float> &b) const
    {
        float sum = 0.0;
        for (size_t i = 0; i < a.size(); i++)
            sum += (a[i] - b[i]) * (a[i] - b[i]);
        return std::sqrt(sum);
    }

    float dotProduct(const std::vector<float> &a, const std::vector<float> &b) const
    {
        float dot = 0.0;
        for (size_t i = 0; i < a.size(); i++)
            dot += a[i] * b[i];
        return dot;
    }

    float computeSimilarity(const std::vector<float> &a, const std::vector<float> &b) const
    {
        if (metric == "cosine")
            return cosineSimilarity(a, b);
        else if (metric == "euclidean")
            // convert distance → similarity (invert)
            return 1.0f / (1.0f + euclideanDistance(a, b));
        else if (metric == "dot")
            return dotProduct(a, b);
        else
            return cosineSimilarity(a, b); // default fallback
    }

public:
    Block(const std::string &metricType = "cosine") : metric(metricType) {}

    void insertRecord(const std::vector<float> &record)
    {
        records.push_back(record);
    }

    std::vector<std::vector<float>> getRecords() const
    {
        return records;
    }

    std::vector<std::pair<int, std::vector<float>>> getRecordsWithId() const
    {
        std::vector<std::pair<int, std::vector<float>>> out;
        for (size_t i = 0; i < records.size(); i++)
            out.push_back({(int)i, records[i]});
        return out;
    }

    std::vector<std::pair<int, float>> findNearest(
        const std::vector<float> &query, int top_k = 1) const
    {
        std::vector<std::pair<int, float>> sims;
        for (size_t i = 0; i < records.size(); i++)
        {
            float score = computeSimilarity(query, records[i]);
            sims.push_back({(int)i, score});
        }

        // sort descending (best similarity first)
        std::sort(sims.begin(), sims.end(),
                  [](auto &a, auto &b)
                  { return a.second > b.second; });

        if ((int)sims.size() > top_k)
            sims.resize(top_k);

        return sims;
    }

    std::string getMetric() const { return metric; }
    void setMetric(const std::string &m) { metric = m; } // ✅ allow changing metric

    // ✅ for persistence
    Json::Value toJson() const
    {
        Json::Value root;
        root["metric"] = metric;
        for (auto &vec : records)
        {
            Json::Value arr(Json::arrayValue);
            for (auto v : vec)
                arr.append(v);
            root["records"].append(arr);
        }
        return root;
    }

    void fromJson(const Json::Value &root)
    {
        metric = root.get("metric", "cosine").asString();
        records.clear();
        for (auto &rec : root["records"])
        {
            std::vector<float> vec;
            for (auto &v : rec)
                vec.push_back(v.asFloat());
            records.push_back(vec);
        }
    }
};
