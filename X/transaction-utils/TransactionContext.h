#pragma once
#include <string>
#include <set>
#include "../keyspace-hashtable/KeySpace.h"

// Session-wide BEGIN/COMMIT/ROLLBACK state. The REPL is single-threaded end to end,
// so a static holder is sufficient — no locking needed.
class TransactionContext
{
public:
    static bool isActive();

    static void begin(const KeySpace &db);
    static void markDirty(const std::string &keyspaceName);
    static void markPendingDelete(const std::string &keyspaceName);
    static bool commit(KeySpace &db);
    static bool rollback(KeySpace &db);

private:
    static bool active;
    static KeySpace snapshot;
    static std::set<std::string> dirtyKeyspaces;
    static std::set<std::string> pendingDeletes;
};
