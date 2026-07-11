#include "insert.h"
#include "../../transaction-utils/Transaction.h"
#include "../../db-storage-util/db-storage-insert.h"
#include "../../common_utils/common_utils.h"
#include "../../insert-utils/insert_line.h"
#include <iostream>

void handleInsert(std::stringstream &ss, KeySpace &db)
{
    std::string keyspace;
    ss >> keyspace;
    std::string rest;
    std::getline(ss, rest);

    if (!db.exists(keyspace))
    {
        std::cout << "FAILURE : Keyspace does not exist\n";
        return;
    }

    ResolvedLine resolved = resolveInsertLine(rest);
    if (!resolved.success)
    {
        std::cout << "FAILURE : " << resolved.error << "\n";
        return;
    }

    Transaction t = execute_insert(db, keyspace, resolved.embedding, resolved.text);
    std::cout << t.getStatus() << " : " << t.getMessage() << "\n";
    if (t.getStatus() == "SUCCESS")
        saveDb(db, keyspace);
}
