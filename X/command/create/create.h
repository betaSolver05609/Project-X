#pragma once
#include <sstream>
#include "../../keyspace-hashtable/KeySpace.h"

// Handle CREATE command
void handleCreate(std::stringstream &ss, KeySpace &db);
