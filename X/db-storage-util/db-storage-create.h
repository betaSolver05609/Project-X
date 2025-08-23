#pragma once
#include <string>
#include "transaction-utils/Transaction.h"
#include "keyspace-hashtable/KeySpace.h"


Transaction create_keyspace(KeySpace& db, const std::string& name) {
    if (db.exists(name)) {
        return Transaction(false, "Keyspace '" + name + "' already exists");
    }

    db.createKeySpace(name);
    return Transaction(true, "Keyspace '" + name + "' created");
}
