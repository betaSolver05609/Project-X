#include "ingest.h"
#include "../../ingest/ingest.h" // the ingestion engine
#include "./keyspace-hashtable/KeySpace.h"
#include <iostream>

void handleIngestCommand(std::stringstream &ss, KeySpace &db)
{
    std::string what, intoWord, keyspaceWord, keyspaceName, filepathExpr;
    ss >> what >> intoWord >> keyspaceWord >> keyspaceName >> filepathExpr;
    if (what != "DATA" || intoWord != "INTO" || keyspaceWord != "KEYSPACE")
    {   
        std::cout << "[DEBUG] something failed" << std::endl;
        std::cout << "Invalid INGEST syntax. Usage:\n"
                  << "INGEST DATA INTO KEYSPACE <name> FILEPATH=\"/path/to/file\"\n";
        return;
    }

    std::string filepath;
    if (filepathExpr.rfind("FILEPATH=", 0) == 0)
    {
        // Strip FILEPATH= and optional quotes
        filepath = filepathExpr.substr(9);
        if (!filepath.empty() && filepath.front() == '"' && filepath.back() == '"')
        {
            filepath = filepath.substr(1, filepath.size() - 2);
        }
    }
    else
    {
        std::cout << "Missing FILEPATH argument.\n";
        return;
    }

    if (!handleIngest(keyspaceName, filepathExpr, db))
    {
        std::cout << "Ingest failed for keyspace '" << keyspaceName
                  << "' with file '" << filepath << "'.\n";
    }
}
