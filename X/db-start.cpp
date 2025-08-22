#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "transaction-utils/Transaction.h"
#include "db-storage-util/db-storage-create.h"
#include "db-storage-util/db-storage-insert.h"
#include "keyspace-hashtable/KeySpace.h"
#include <fstream>

#include <filesystem> // for create_directory

void saveDb(const KeySpace &db)
{
    std::filesystem::create_directory("data");

    for (auto &name : db.listKeyspaces())
    {
        Json::Value root = db.toJson(name);
        std::ofstream file("data/" + name + ".json");
        if (file.is_open())
        {
            Json::StreamWriterBuilder writer;
            writer["indentation"] = "  ";
            file << Json::writeString(writer, root);
        }
    }
}

void loadDb(KeySpace &db)
{
    std::filesystem::create_directory("data");

    for (auto &entry : std::filesystem::directory_iterator("data"))
    {
        if (entry.path().extension() == ".json")
        {
            std::ifstream file(entry.path());
            if (file.is_open())
            {
                Json::Value root;
                file >> root;
                std::string name = entry.path().stem().string(); // filename without .json
                db.fromJson(name, root);
            }
        }
    }
}

std::vector<float> parseVector(const std::string &input)
{
    std::vector<float> vec;
    std::stringstream ss(input);
    char c;
    float num;
    while (ss >> c)
    {
        if ((c >= '0' && c <= '9') || c == '-' || c == '.')
        {
            ss.putback(c);
            ss >> num;
            vec.push_back(num);
        }
    }
    return vec;
}

int main()
{
    KeySpace db;
    loadDb(db);
    std::string line;

    std::cout << "Welcome to Project-X VectorDB CLI \n";

    while (true)
    {
        std::cout << ">> ";
        if (!std::getline(std::cin, line))
            break;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "EXIT")
        {
            saveDb(db);
            break;
        }
        else if (cmd == "CREATE")
        {
            std::string what;
            ss >> what;
            if (what == "KEYSPACE")
            {
                std::string keyspace;
                ss >> keyspace;

                std::string withWord, metricWord, metricType = "cosine";

                if (ss >> withWord >> metricWord >> metricType) // expects: WITH METRIC <type>
                {
                    if (withWord != "WITH" || metricWord != "METRIC")
                    {
                        metricType = "cosine"; // fallback
                    }
                }

                db.createKeySpace(keyspace, metricType);
                std::cout << "Keyspace '" << keyspace
                          << "' created with metric '" << metricType << "'.\n";
                saveDb(db);
            }
        }

        else if (cmd == "INSERT")
        {
            std::string keyspace;
            ss >> keyspace;
            std::string vecStr;
            std::getline(ss, vecStr);
            auto vec = parseVector(vecStr);
            Transaction t = execute_insert(db, keyspace, vec);
            std::cout << t.getStatus() << " : " << t.getMessage() << "\n";

            saveDb(db); // ✅ persist immediately
        }
        else if (cmd == "SEARCH")
        {
            std::string keyspace;
            ss >> keyspace;
            std::string vecStr;
            std::getline(ss, vecStr, 'T'); // read until "TOP"
            auto query = parseVector(vecStr);
            std::string topWord;
            int topk = 1;
            if (ss >> topWord >> topk)
            {
            } // parse TOP k if provided

            auto results = db.searchInKeyspace(keyspace, query, topk);
            if (results.empty())
            {
                std::cout << "(no matching records found in '" << keyspace << "')\n";
            }
            else
            {
                for (auto &r : results)
                {
                    std::cout << "Record Index: " << r.first
                              << " | Similarity: " << r.second << "\n";
                }
            }
        }

        else if (cmd == "LIST")
        {
            std::string what;
            ss >> what;
            if (what == "KEYSPACES")
            {
                auto names = db.listKeyspaces();
                if (names.empty())
                {
                    std::cout << "(no keyspaces found)\n";
                }
                else
                {
                    for (auto &n : names)
                    {
                        std::cout << "- " << n << "\n";
                    }
                }
            }
        }
        else if (cmd == "ALTER")
        {
            std::string what;
            ss >> what;
            if (what == "KEYSPACE")
            {
                std::string name, setWord, metricWord, metricType;
                ss >> name >> setWord >> metricWord >> metricType;
                if (setWord == "SET" && metricWord == "METRIC")
                {
                    if (db.alterMetric(name, metricType))
                    {
                        std::cout << "Keyspace '" << name
                                  << "' metric changed to '" << metricType << "'.\n";
                        saveDb(db); // persist update
                    }
                    else
                    {
                        std::cout << "Keyspace '" << name << "' does not exist.\n";
                    }
                }
            }
        }

        else if (cmd == "SHOW")
        {
            std::string what, keyspace;
            ss >> what >> keyspace;
            if (what == "METRIC")
            {
                std::string name;
                ss >> name;
                std::string metric = db.getMetric(name);
                if (!metric.empty())
                {
                    std::cout << "Keyspace '" << name
                              << "' is using metric '" << metric << "'.\n";
                }
                else
                {
                    std::cout << "Keyspace '" << name << "' does not exist.\n";
                }
            }
            if (what == "RECORDS")
            {
                auto records = db.getRecords(keyspace);
                if (records.empty())
                {
                    std::cout << "(no records found in '" << keyspace << "')\n";
                }
                else
                {
                    int idx = 0;
                    for (auto &r : records)
                    {
                        std::cout << "Record " << idx++ << ": [ ";
                        for (auto v : r)
                            std::cout << v << " ";
                        std::cout << "]\n";
                    }
                }
            }
        }
        else if (cmd == "DROP")
        {
            std::string what;
            ss >> what;
            if (what == "KEYSPACE")
            {
                std::string name;
                ss >> name;
                bool dropped = db.dropKeySpace(name);
                if (dropped)
                {
                    // delete persisted file
                    std::string path = "data/" + name + ".json";
                    if (std::remove(path.c_str()) == 0)
                        std::cout << "Keyspace '" << name << "' dropped and file deleted.\n";
                    else
                        std::cout << "Keyspace '" << name << "' dropped (file not found).\n";

                    saveDb(db); // persist updated state
                }
                else
                {
                    std::cout << "Keyspace '" << name << "' does not exist.\n";
                }
            }
        }

        else if (cmd == "SHOW")
        {
            std::string what;
            ss >> what;
            if (what == "RECORDS")
            {
                std::string name;
                ss >> name;
                auto records = db.getRecords(name);
                if (records.empty())
                {
                    std::cout << "(no records found in '" << name << "')\n";
                }
                else
                {
                    int idx = 0;
                    for (auto &r : records)
                    {
                        std::cout << "Record " << idx++ << ": [ ";
                        for (auto v : r)
                            std::cout << v << " ";
                        std::cout << "]\n";
                    }
                }
            }
        }
        else
        {
            std::cout << "Unknown command.\n";
        }
    }
    std::cout << "Bye 👋\n";
    return 0;
}
