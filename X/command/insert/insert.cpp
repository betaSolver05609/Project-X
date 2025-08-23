#include "insert.h"
#include "../../transaction-utils/Transaction.h"
#include "../../db-storage-util/db-storage-insert.h"
#include "../../common_utils/common_utils.h"
#include <iostream>

void handleInsert(std::stringstream &ss, KeySpace &db)
{
    std::string keyspace;
    ss >> keyspace;
    std::string vecStr;
    std::getline(ss, vecStr);

    auto vec = parseVector(vecStr);
    Transaction t = execute_insert(db, keyspace, vec);

    std::cout << t.getStatus() << " : " << t.getMessage() << "\n";

    saveDb(db); 
}
