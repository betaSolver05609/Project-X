#include "drop.h"
#include <iostream>
#include <cstdio>
#include "../../common_utils/common_utils.h"
#include "../../transaction-utils/TransactionContext.h"

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
            if (TransactionContext::isActive())
            {
                // Defer the physical file removal to COMMIT so a ROLLBACK can still
                // recover it; the in-memory keyspace is already gone for this session.
                TransactionContext::markPendingDelete(name);
                std::cout << "Keyspace '" << name << "' dropped (pending commit).\n";
            }
            else
            {
                std::string path = "data/" + name + ".bin";
                if (std::remove(path.c_str()) == 0)
                    std::cout << "Keyspace '" << name << "' dropped and file deleted.\n";
                else
                    std::cout << "Keyspace '" << name << "' dropped (file not found).\n";
            }
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
