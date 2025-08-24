#include "ingest.h"
#include "../ingest/parsers/csv_parser.h"
#include "../ingest/parsers/json_parser.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include "common_utils/json_portable.h"

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

    std::vector<std::vector<float>> records;

    // --- Detect file extension ---
    if (filepath.size() >= 5 && filepath.substr(filepath.size()-5) == ".json") {
        records = parseJsonFile(filepath);
    }
    else if (filepath.size() >= 4 && filepath.substr(filepath.size()-4) == ".csv") {
        records = parseCsvFile(filepath);
    }
    else {
        std::cerr << "Unsupported file format: " << filepath << "\n";
        return false;
    }

    int total = records.size();
    if (total == 0)
    {
        std::cout << "No records found in file.\n";
        return true;
    }

    int count = 0;
    for (auto &vec : records)
    {
        db.insertIntoBlock(keyspaceName, vec);
        count++;

        // --- Progress bar ---
        int percent = (count * 100) / total;
        int barWidth = 50;
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
