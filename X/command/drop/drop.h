#pragma once
#include <sstream>
#include "../../keyspace-hashtable/KeySpace.h"

// Handle DROP command
void handleDrop(std::stringstream &ss, KeySpace &db);
