#pragma once
#include <sstream>
#include "../../keyspace-hashtable/KeySpace.h"

// Handle ALTER command
void handleAlter(std::stringstream &ss, KeySpace &db);
