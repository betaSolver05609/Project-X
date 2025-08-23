#include "dispatcher.h"
#include "../command/insert/insert.h"
#include "../command/create/create.h"
#include "../command/search/search.h"
#include "../command/list/list.h"
#include "../command/alter/alter.h"
#include "../command/show/show.h"
#include "../command/drop/drop.h"
#include "../command/exit/exit.h"
#include "../common_utils/common_utils.h"
#include <iostream>

bool dispatchCommand(std::stringstream &ss, KeySpace &db)
{
    std::string cmd;
    ss >> cmd;

    if (cmd == "CREATE")
        handleCreate(ss, db);
    else if (cmd == "INSERT")
        handleInsert(ss, db);
    else if (cmd == "SEARCH")
        handleSearch(ss, db);
    else if (cmd == "LIST")
        handleList(ss, db);
    else if (cmd == "ALTER")
        handleAlter(ss, db);
    else if (cmd == "SHOW")
        handleShow(ss, db);
    else if (cmd == "DROP")
        handleDrop(ss, db);
    else if (cmd == "EXIT")
        return handleExit(ss, db);
    else
        std::cout << "Unknown command.\n";

    return false; // keep running
}
