#pragma once
#include <sstream>
#include "../../keyspace-hashtable/KeySpace.h"

void handleRollback(std::stringstream &ss, KeySpace &db);
