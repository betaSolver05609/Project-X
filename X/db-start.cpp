#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "transaction-utils/Transaction.h"
#include "keyspace-hashtable/KeySpace.h"
#include <fstream>
#include "common_utils/common_utils.h"
#include "handler/dispatcher.h"

int main()
{
    KeySpace db;
    loadDb(db);
    std::string line;

    std::cout << "Welcome to Project-X VectorDB CLI \n";

    while (true)
    {
        std::cout << ">> ";
        if (!std::getline(std::cin, line))
            break;

        std::stringstream ss(line);

        // Dispatcher now handles parsing + routing
        if (dispatchCommand(ss, db))
            break;  // EXIT was called
    }

    std::cout << "Bye 👋\n";
    return 0;
}
