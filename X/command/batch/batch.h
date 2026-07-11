#pragma once
#include <sstream>
#include "../../keyspace-hashtable/KeySpace.h"

void handleBatch(std::stringstream &ss, KeySpace &db);
