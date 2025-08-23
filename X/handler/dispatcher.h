#pragma once
#include <string>
#include <sstream>
#include "../keyspace-hashtable/KeySpace.h"


bool dispatchCommand(std::stringstream &ss, KeySpace &db);

