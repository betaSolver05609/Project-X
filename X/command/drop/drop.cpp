#include "drop.h"
#include <iostream>
#include <cstdio>   // for std::remove
#include "../../common_utils/common_utils.h"

void handleDrop(std::stringstream &ss, KeySpace &db)
{
    std::string what;
    ss >> what;

    if (what == "KEYSPACE")
    {
        std::string name;
        ss >> name;
        bool dropped = db.dropKeySpace(name);

        if (dropped)
        {
            // delete persisted file
            std::string path = "data/" + name + ".json";
            if (std::remove(path.c_str()) == 0)
                std::cout << "Keyspace '" << name << "' dropped and file deleted.\n";
            else
                std::cout << "Keyspace '" << name << "' dropped (file not found).\n";

            saveDb(db); // persist updated state
        }
        else
        {
            std::cout << "Keyspace '" << name << "' does not exist.\n";
        }
    }
    else
    {
        std::cout << "Unknown DROP option. Use DROP KEYSPACE <name>.\n";
    }
}
