#pragma once
#include <sstream>
#include "../../keyspace-hashtable/KeySpace.h"

// Handle EXIT command
// Returns true if the program should exit
bool handleExit(std::stringstream &ss, KeySpace &db);
