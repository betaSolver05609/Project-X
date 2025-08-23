#include "ingest.h"
#include <fstream>
#include <iostream>
#include <iomanip> // for std::setw
#include <jsoncpp/json/json.h>

bool handleIngest(std::string keyspaceName, std::string filepathArg, KeySpace &db)
{
    // Extract filepath=...
    std::string filepath;
    auto pos = filepathArg.find("FILEPATH=");
    if (pos != std::string::npos)
    {
        filepath = filepathArg.substr(pos + 9);
        if (!filepath.empty() && filepath.front() == '"')
            filepath = filepath.substr(1, filepath.size() - 2); // strip quotes
    }

    if (filepath.empty())
    {
        std::cerr << "Filepath not provided.\n";
        return false;
    }

    if (!db.exists(keyspaceName))
    {
        std::cerr << "Keyspace '" << keyspaceName << "' does not exist.\n";
        return false;
    }

    // Load JSON
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filepath << "\n";
        return false;
    }

    Json::Value root;
    file >> root;

    if (!root.isMember("records") || !root["records"].isArray())
    {
        std::cerr << "Invalid file format. Must contain { \"records\": [[...], ...] }\n";
        return false;
    }

    int total = root["records"].size();
    if (total == 0)
    {
        std::cout << "No records found in file.\n";
        return true;
    }

    int count = 0;
    for (auto &rec : root["records"])
    {
        std::vector<float> vec;
        for (auto &v : rec)
            vec.push_back(v.asFloat());

        db.insertIntoBlock(keyspaceName, vec);
        count++;

        // --- Progress bar ---
        int percent = (count * 100) / total;
        int barWidth = 50; // length of progress bar
        int filled = (percent * barWidth) / 100;

        std::cout << "\r[";
        for (int i = 0; i < barWidth; i++)
        {
            if (i < filled)
                std::cout << "=";
            else if (i == filled)
                std::cout << ">";
            else
                std::cout << " ";
        }
        std::cout << "] " << std::setw(3) << percent << "% (" << count << "/" << total << ")"
                  << std::flush;
    }

    std::cout << "\nIngested " << count << " records into keyspace '" << keyspaceName << "'.\n";
    return true;
}
