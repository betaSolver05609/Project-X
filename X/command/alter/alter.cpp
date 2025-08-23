
#include "alter.h"
#include "../../common_utils/common_utils.h"
#include <iostream>

void handleAlter(std::stringstream &ss, KeySpace &db)
{
    std::string what;
    ss >> what;

    if (what == "KEYSPACE")
    {
        std::string name, setWord, metricWord, metricType;
        ss >> name >> setWord >> metricWord >> metricType;

        if (setWord == "SET" && metricWord == "METRIC")
        {
            if (db.alterMetric(name, metricType))
            {
                std::cout << "Keyspace '" << name
                          << "' metric changed to '" << metricType << "'.\n";
                saveDb(db); 
            }
            else
            {
                std::cout << "Keyspace '" << name << "' does not exist.\n";
            }
        }
        else
        {
            std::cout << "Invalid ALTER syntax. Usage: ALTER KEYSPACE <name> SET METRIC <type>\n";
        }
    }
    else
    {
        std::cout << "Unknown ALTER option.\n";
    }
}
