#include "TransactionContext.h"
#include "../common_utils/common_utils.h"
#include <cstdio>

bool TransactionContext::active = false;
KeySpace TransactionContext::snapshot;
std::set<std::string> TransactionContext::dirtyKeyspaces;
std::set<std::string> TransactionContext::pendingDeletes;

bool TransactionContext::isActive()
{
    return active;
}

void TransactionContext::begin(const KeySpace &db)
{
    snapshot = db;
    dirtyKeyspaces.clear();
    pendingDeletes.clear();
    active = true;
}

void TransactionContext::markDirty(const std::string &keyspaceName)
{
    dirtyKeyspaces.insert(keyspaceName);
    pendingDeletes.erase(keyspaceName);
}

void TransactionContext::markPendingDelete(const std::string &keyspaceName)
{
    pendingDeletes.insert(keyspaceName);
    dirtyKeyspaces.erase(keyspaceName);
}

bool TransactionContext::commit(KeySpace &db)
{
    if (!active)
        return false;

    std::set<std::string> toSave = dirtyKeyspaces;
    std::set<std::string> toDelete = pendingDeletes;

    active = false;
    dirtyKeyspaces.clear();
    pendingDeletes.clear();

    for (auto &name : toSave)
        saveDb(db, name);

    for (auto &name : toDelete)
    {
        std::string path = "data/" + name + ".bin";
        std::remove(path.c_str());
    }

    return true;
}

bool TransactionContext::rollback(KeySpace &db)
{
    if (!active)
        return false;

    db = snapshot;
    active = false;
    dirtyKeyspaces.clear();
    pendingDeletes.clear();

    return true;
}
