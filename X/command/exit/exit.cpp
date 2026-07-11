#include "exit.h"
#include <iostream>
#include "../../common_utils/common_utils.h"
#include "../../transaction-utils/TransactionContext.h"

bool handleExit(std::stringstream &ss, KeySpace &db)
{
    (void)ss;

    if (TransactionContext::isActive())
    {
        std::cout << "Warning: an open transaction was not committed; rolling it back.\n";
        TransactionContext::rollback(db);
    }

    saveDb(db);
    std::cout << "Exiting. Database saved.\n";
    return true;
}
