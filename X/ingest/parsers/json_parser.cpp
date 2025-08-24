#include "json_parser.h"
#include "common_utils/json_portable.h"
#include <fstream>
#include <iostream>

std::vector<std::vector<float>> parseJsonFile(const std::string& filepath)
{
    std::vector<std::vector<float>> result;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON file: " << filepath << "\n";
        return result;
    }

    Json::Value root;
    file >> root;

    if (!root.isArray()) {
        std::cerr << "JSON root must be an array of vectors\n";
        return result;
    }

    for (const auto& item : root) {
        if (!item.isArray()) continue;
        std::vector<float> vec;
        for (const auto& val : item) {
            vec.push_back(val.asFloat());
        }
        result.push_back(vec);
    }

    return result;
}
