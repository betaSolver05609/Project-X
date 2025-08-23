#include "create.h"
#include "../../common_utils/common_utils.h"
#include <iostream>

void handleCreate(std::stringstream &ss, KeySpace &db)
{
    std::string what;
    ss >> what;
    if (what == "KEYSPACE")
    {
        std::string keyspace;
        ss >> keyspace;

        std::string withWord, metricWord, metricType = "cosine";

        
        if (ss >> withWord >> metricWord >> metricType)
        {
            if (withWord != "WITH" || metricWord != "METRIC")
            {
                metricType = "cosine"; 
            }
        }

        db.createKeySpace(keyspace, metricType);
        std::cout << "Keyspace '" << keyspace
                  << "' created with metric '" << metricType << "'.\n";
        saveDb(db);
    }
}
