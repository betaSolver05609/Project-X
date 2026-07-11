#pragma once
#include <string>
#include <vector>
#include "keyspace-hashtable/KeySpace.h"


std::vector<float> parseVector(const std::string &input);


void saveDb(const KeySpace &db);

void saveDb(const KeySpace &db, const std::string &keyspaceName);

void loadDb(KeySpace &db);
