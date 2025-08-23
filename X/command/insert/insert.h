#pragma once
#include <sstream>
#include "../../keyspace-hashtable/KeySpace.h"

// Handle INSERT command
void handleInsert(std::stringstream &ss, KeySpace &db);
