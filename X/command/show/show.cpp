#include "show.h"
#include <iostream>

void handleShow(std::stringstream &ss, KeySpace &db)
{
    std::string what, keyspace;
    ss >> what >> keyspace;

    if (what == "METRIC")
    {
        std::string name;
        ss >> name;
        std::string metric = db.getMetric(name);
        if (!metric.empty())
        {
            std::cout << "Keyspace '" << name
                      << "' is using metric '" << metric << "'.\n";
        }
        else
        {
            std::cout << "Keyspace '" << name << "' does not exist.\n";
        }
    }
    else if (what == "RECORDS")
    {
        auto records = db.getRecords(keyspace);
        if (records.empty())
        {
            std::cout << "(no records found in '" << keyspace << "')\n";
        }
        else
        {
            int idx = 0;
            for (auto &r : records)
            {
                std::cout << "Record " << idx++ << ": [ ";
                for (auto v : r)
                    std::cout << v << " ";
                std::cout << "]\n";
            }
        }
    }
    else
    {
        std::cout << "Unknown SHOW option. Use SHOW METRIC <keyspace> or SHOW RECORDS <keyspace>.\n";
    }
}
 