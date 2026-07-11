#include "begin.h"
#include "../../transaction-utils/TransactionContext.h"
#include <iostream>

void handleBegin(std::stringstream &ss, KeySpace &db)
{
    (void)ss;

    if (TransactionContext::isActive())
    {
        std::cout << "A transaction is already in progress.\n";
        return;
    }

    TransactionContext::begin(db);
    std::cout << "Transaction started.\n";
}
