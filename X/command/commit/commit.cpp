#include "commit.h"
#include "../../transaction-utils/TransactionContext.h"
#include <iostream>

void handleCommit(std::stringstream &ss, KeySpace &db)
{
    (void)ss;

    if (!TransactionContext::commit(db))
    {
        std::cout << "No transaction in progress.\n";
        return;
    }

    std::cout << "Transaction committed.\n";
}
