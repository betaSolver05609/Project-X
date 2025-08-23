#pragma once
#include <sstream>
#include "../../keyspace-hashtable/KeySpace.h"

// CLI handler for the INGEST command
void handleIngestCommand(std::stringstream &ss, KeySpace &db);
