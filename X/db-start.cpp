#include <sched.h>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include "db-storage-util/db-storage-create.h"
#include "db-storage-util/db-storage-insert.h"
using namespace std;

Transaction execute_transaction(std::string inputCommand) {
    Transaction transaction;
    if(inputCommand.find("CREATE")!=std::string::npos) {
        transaction=execute_create(inputCommand);
    }
    else if(inputCommand.find("INSERT")!=std::string::npos) {
        transaction=execute_insert(inputCommand);
    }
    return transaction;
}

int main() {
    std::string inputCommand;
    cout<<"SESSION CREATED"<<endl;
    if(inputCommand.find("TRANSACTION START")) {
    while(true) {
        getline(cin, inputCommand);
        if(inputCommand=="TRANSACTION END") {
            break;
        }
        Transaction obj;
        obj=execute_transaction(inputCommand);
        if(obj.getTransactionStatus()==200) {
            std::cout<<"DONE"<<endl;
        }
    }
    }
    return 0;
}