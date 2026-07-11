#pragma once
#include <sstream>
#include "../../keyspace-hashtable/KeySpace.h"

void handleCommit(std::stringstream &ss, KeySpace &db);
