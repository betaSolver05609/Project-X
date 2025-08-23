#include "list.h"
#include <iostream>

void handleList(std::stringstream &ss, KeySpace &db)
{
    std::string what;
    ss >> what;

    if (what == "KEYSPACES")
    {
        auto names = db.listKeyspaces();
        if (names.empty())
        {
            std::cout << "(no keyspaces found)\n";
        }
        else
        {
            for (auto &n : names)
            {
                std::cout << "- " << n << "\n";
            }
        }
    }
    else
    {
        std::cout << "Unknown LIST option.\n";
    }
}
