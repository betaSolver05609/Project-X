#pragma once
#include <string>
#include <vector>
#include <jsoncpp/json/json.h>
#include "keyspace-hashtable/KeySpace.h"

// Parse a string into a vector<float>
std::vector<float> parseVector(const std::string &input);

// Save DB to JSON files
void saveDb(const KeySpace &db);

// Load DB from JSON files
void loadDb(KeySpace &db);
