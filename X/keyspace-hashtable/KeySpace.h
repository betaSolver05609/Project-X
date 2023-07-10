#ifndef KEYSPACE_H
#define KEYSPACE_H
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
class KeySpace {
    public:
        std::unordered_map<std::string, std::vector<std::string>> keyspace;
        KeySpace(){}
        KeySpace(std::string keyspace_name, std::vector<std::string> blocks) {
            keyspace[keyspace_name]=blocks;
        }
        std::vector<std::string> getKeySpaceBlock(std::string keyspace_name) {
            return keyspace[keyspace_name];
        }
        void setKeySpaceBlock(std::string keyspaceName, std::vector<std::string> blocks) {
            keyspace[keyspaceName]=blocks;
        }
};

std::unordered_map<std::string, std::vector<std::string>> temp;
#endif