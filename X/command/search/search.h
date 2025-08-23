#pragma once
#include <sstream>
#include "../../keyspace-hashtable/KeySpace.h"

// Handle SEARCH command
void handleSearch(std::stringstream &ss, KeySpace &db);
