#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <memory>
#include <jsoncpp/json/json.h>
#include "similarity-engines/factory.h"

class Block
{
private:
    std::vector<std::vector<float>> records;
    std::string metric;
    std::unique_ptr<SimilarityEngine> engine;

public:
    // Constructor
    Block(const std::string &metricType = "cosine")
        : metric(metricType), engine(createSimilarityEngine(metricType)) {}

    // Deep copy constructor
    Block(const Block &other)
        : records(other.records), metric(other.metric),
          engine(createSimilarityEngine(other.metric)) {}

    // Deep copy assignment operator
    Block &operator=(const Block &other)
    {
        if (this != &other)
        {
            records = other.records;
            metric = other.metric;
            engine = createSimilarityEngine(other.metric); // re-create engine
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
    }

    std::string getMetric() const { return metric; }

    // Record management
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

    // Nearest neighbor search
    std::vector<std::pair<int, float>> findNearest(const std::vector<float> &query, int top_k = 1) const
    {
        std::vector<std::pair<int, float>> sims;
        for (size_t i = 0; i < records.size(); i++)
        {
            float sim;
            if (metric == "MAHALABONIS")
            {
                sim = engine->computeWithDataset(query, records[i], records);
            }
            else
            {
                sim = engine->compute(query, records[i]);
            }
            sims.emplace_back(i, sim);
        }

        std::sort(sims.begin(), sims.end(),
                  [](auto &a, auto &b)
                  { return a.second > b.second; });

        if ((int)sims.size() > top_k)
            sims.resize(top_k);

        return sims;
    }

    // JSON serialization
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
        engine = createSimilarityEngine(metric);
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
