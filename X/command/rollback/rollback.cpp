#include "rollback.h"
#include "../../transaction-utils/TransactionContext.h"
#include <iostream>

void handleRollback(std::stringstream &ss, KeySpace &db)
{
    (void)ss;

    if (!TransactionContext::rollback(db))
    {
        std::cout << "No transaction in progress.\n";
        return;
    }

    std::cout << "Transaction rolled back.\n";
}
