#include "search.h"
#include "../../common_utils/common_utils.h"
#include <iostream>

void handleSearch(std::stringstream &ss, KeySpace &db) {
    std::string keyspace;
    ss >> keyspace;

    std::string vecStr;
    std::getline(ss, vecStr, 'T'); // read until "TOP"
    auto query = parseVector(vecStr);

    std::string topWord;
    int topk = 1;
    if (ss >> topWord >> topk) {
        // parsed TOP k
    }

    auto results = db.searchInKeyspace(keyspace, query, topk);
    if (results.empty()) {
        std::cout << "(no matching records found in '" << keyspace << "')\n";
    } else {
        for (auto &r : results) {
            std::cout << "Record Index: " << r.first
                      << " | Similarity: " << r.second << "\n";
        }
    }
}
