#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "../keyspace-hashtable/KeySpace.h"
#include "../transaction-utils/Transaction.h"
using namespace std;

Transaction create_keyspace(std::string inputCommand) {
    stringstream stream(inputCommand);
    std:vector<std::string> blocks;
    std::string lastWord, word;
    while(stream>>word) {
        lastWord=word;
    }
    KeySpace keyspace=KeySpace(lastWord, blocks);
    blocks=keyspace.getKeySpaceBlock(lastWord);
    std::string transactionMessage="keyspace "+lastWord+" created successfully";
    Transaction transaction=Transaction(transactionMessage, 200);
    return transaction;
}
Transaction create_block(std::string inputCommand) {
    stringstream stream(inputCommand);
    std::string lastWord, word, blockName, keyspaceName;
    while(stream>>word){
        if(lastWord=="BLOCK"){
            blockName=word;
        }
        if(lastWord=="UNDER") {
            keyspaceName=lastWord;
        }
        lastWord=word;
        KeySpace keyspace;
        std::vector<std::string> temp=keyspace.getKeySpaceBlock(keyspaceName);
        temp.push_back(blockName);
        keyspace.setKeySpaceBlock(keyspaceName, temp);
        std::string transactionMessage="Block "+blockName+" created successfully";
        Transaction transaction=Transaction(transactionMessage, 200);
        return transaction;
    }
}
Transaction execute_create(std::string inputCommand) {
    Transaction transaction;
    if(inputCommand.find("KEYSPACE")!=std::string::npos && inputCommand.find("BLOCK")==std::string::npos) {
        transaction=create_keyspace(inputCommand);
    }
    else {
        transaction=create_block(inputCommand);
    }
    return transaction;
}

