#include "csv_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<std::vector<float>> parseCsvFile(const std::string& filepath)
{
    std::vector<std::vector<float>> result;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open CSV file: " << filepath << "\n";
        return result;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<float> row;

        while (std::getline(ss, cell, ',')) {
            try {
                row.push_back(std::stof(cell));
            } catch (...) {
                std::cerr << "Invalid float in CSV: " << cell << "\n";
            }
        }

        if (!row.empty()) {
            result.push_back(row);
        }
    }

    return result;
}
