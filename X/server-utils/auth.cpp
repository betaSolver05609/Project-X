#include "auth.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

UserStore UserStore::loadOrCreate(const std::string &path)
{
    UserStore store;

    std::filesystem::path filePath(path);
    if (!filePath.parent_path().empty())
        std::filesystem::create_directories(filePath.parent_path());

    if (!std::filesystem::exists(filePath))
    {
        std::ofstream out(filePath, std::ios::trunc);
        out << "admin:admin\n";
        std::cout << "No credentials file found; created " << path
                  << " with a default admin:admin account. Change this before real use.\n";
    }

    std::ifstream in(filePath);
    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty())
            continue;

        auto sep = line.find(':');
        if (sep == std::string::npos)
            continue;

        std::string user = line.substr(0, sep);
        std::string pass = line.substr(sep + 1);
        if (!user.empty())
            store.credentials[user] = pass;
    }

    return store;
}

bool UserStore::authenticate(const std::string &username, const std::string &password) const
{
    auto it = credentials.find(username);
    if (it == credentials.end())
        return false;
    return it->second == password;
}
