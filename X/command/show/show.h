#pragma once
#include <sstream>
#include "../../keyspace-hashtable/KeySpace.h"

// Handle SHOW command
void handleShow(std::stringstream &ss, KeySpace &db);
