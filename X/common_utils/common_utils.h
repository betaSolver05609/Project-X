#pragma once
#include <string>
#include <vector>
#include <jsoncpp/json/json.h>
#include "keyspace-hashtable/KeySpace.h"


std::vector<float> parseVector(const std::string &input);


void saveDb(const KeySpace &db);


void loadDb(KeySpace &db);
