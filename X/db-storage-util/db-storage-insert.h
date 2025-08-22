#pragma once
#include <vector>
#include <string>
#include "transaction-utils/Transaction.h"
#include "keyspace-hashtable/KeySpace.h"

Transaction execute_insert(KeySpace& db, const std::string& keyspaceName, const std::vector<float>& record) {
    if (!db.exists(keyspaceName)) {
        return Transaction(false, "Keyspace does not exist");
    }

    db.insertIntoBlock(keyspaceName, record);
    return Transaction(true, "Inserted record into keyspace '" + keyspaceName + "'");
}
