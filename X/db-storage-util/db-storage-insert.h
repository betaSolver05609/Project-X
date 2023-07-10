#include <sstream>
#include <string>
#include <vector>
#include "../transaction-utils/Transaction.h"
#include "../keyspace-hashtable/KeySpace.h"
using namespace std;

Transaction execute_insert(std::string inputCommand) {
    std::string keyspaceName, blockName, data, word, lastWord;
    std::stringstream stream(inputCommand);
    while(stream>>word) {
        if(lastWord.find("BLOCK")) {
            blockName=word;
        }
        if(lastWord.find("KEYSPACE")) {
            keyspaceName=word;
        }
        lastWord=word;
    }
    data=lastWord;
    std::vector<std::string> temp=KeySpace::getKeySpaceBlock(keyspaceName);
    temp.push_back(data);
    KeySpace::setKeySpaceBlock(keyspaceName, temp);
    std::string str(temp.begin(), temp.end());
    std::string str="Hello World";
    Transaction transaction=Transaction(str, 200);
    return transaction;
}

