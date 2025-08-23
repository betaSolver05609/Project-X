#pragma once
#include <string>
#include <sstream>
#include "../keyspace-hashtable/KeySpace.h"

bool handleIngest(std::string keyspaceName, std::string filepathArg,  KeySpace &db);
