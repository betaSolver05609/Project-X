#pragma once
#include <sstream>
#include "../../keyspace-hashtable/KeySpace.h"

// Handle LIST command
void handleList(std::stringstream &ss, KeySpace &db);
