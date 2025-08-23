#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <jsoncpp/json/json.h>
#include "block/Block.h"

class KeySpace
{
private:
    std::unordered_map<std::string, Block> keyspaceBlocks;

public:
    KeySpace() {}

    void createKeySpace(const std::string &name, const std::string &metric = "cosine")
    {
        keyspaceBlocks[name] = Block(metric);
    }

    void insertIntoBlock(const std::string &keyspaceName, const std::vector<float> &record)
    {
        keyspaceBlocks[keyspaceName].insertRecord(record);
    }

    
    std::vector<std::pair<int, float>> searchInKeyspace(
        const std::string &keyspaceName,
        const std::vector<float> &queryVector,
        int top_k = 1) const
    {
        auto it = keyspaceBlocks.find(keyspaceName);
        if (it != keyspaceBlocks.end())
        {
            return it->second.findNearest(queryVector, top_k);
        }
        return {}; 
    }

    
    bool exists(const std::string &name) const
    {
        return keyspaceBlocks.find(name) != keyspaceBlocks.end();
    }

    
    std::vector<std::string> listKeyspaces() const
    {
        std::vector<std::string> names;
        for (const auto &kv : keyspaceBlocks)
        {
            names.push_back(kv.first);
        }
        return names;
    }

    
    std::vector<std::vector<float>> getRecords(const std::string &keyspaceName) const
    {
        auto it = keyspaceBlocks.find(keyspaceName);
        if (it != keyspaceBlocks.end())
        {
            return it->second.getRecords();
        }
        return {};
    }

    bool dropKeySpace(const std::string &name)
    {
        auto it = keyspaceBlocks.find(name);
        if (it != keyspaceBlocks.end())
        {
            keyspaceBlocks.erase(it);
            return true; 
        }
        return false; 
    }

    std::vector<std::pair<int, std::vector<float>>> getRecordsWithId(const std::string &keyspaceName) const
    {
        auto it = keyspaceBlocks.find(keyspaceName);
        if (it != keyspaceBlocks.end())
        {
            return it->second.getRecordsWithId();
        }
        return {};
    }

    Json::Value toJson(const std::string &name) const
    {
        Json::Value root;
        auto it = keyspaceBlocks.find(name);
        if (it != keyspaceBlocks.end())
        {
            
            root["metric"] = it->second.getMetric();

            
            for (auto &vec : it->second.getRecords())
            {
                Json::Value arr(Json::arrayValue);
                for (auto v : vec)
                    arr.append(v);
                root["records"].append(arr);
            }
        }
        return root;
    }

    void fromJson(const std::string &name, const Json::Value &root)
    {
        
        std::string metric = root.isMember("metric")
                                 ? root["metric"].asString()
                                 : "cosine";

        Block block(metric);

        
        for (auto &rec : root["records"])
        {
            std::vector<float> vec;
            for (auto &v : rec)
                vec.push_back(v.asFloat());
            block.insertRecord(vec);
        }

        keyspaceBlocks[name] = block;
    }

    bool alterMetric(const std::string &name, const std::string &metricType)
    {
        auto it = keyspaceBlocks.find(name);
        if (it != keyspaceBlocks.end())
        {
            it->second.setMetric(metricType); 
            return true;
        }
        return false;
    }
    std::string getMetric(const std::string &name) const
    {
        auto it = keyspaceBlocks.find(name);
        if (it != keyspaceBlocks.end())
        {
            return it->second.getMetric();
        }
        return ""; 
    }
};
