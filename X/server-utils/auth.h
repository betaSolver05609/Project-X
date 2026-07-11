#pragma once
#include <string>
#include <map>

// Plaintext username:password store for this pass — no hashing, no TLS on the wire.
// Good enough to gate a connection; not a hardened credential store.
class UserStore
{
public:
    // Loads users from `path`. If the file doesn't exist, creates it with a default
    // admin:admin account and prints a warning to change it.
    static UserStore loadOrCreate(const std::string &path);

    bool authenticate(const std::string &username, const std::string &password) const;

private:
    std::map<std::string, std::string> credentials;
};
