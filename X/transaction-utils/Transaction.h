#ifndef TRANSACTION_H
#define TRANSACTION_H
#pragma once
#include <string>
using namespace std;

class Transaction {
    private:
        std::string transactionMessage;
        int transactionStatus;
    public:
        Transaction() {}
        Transaction(std::string tm, int status) {
            transactionMessage=tm;
            transactionStatus=status;
        }
        int getTransactionStatus() {
            return transactionStatus;
        }
        std::string getTransactionMessage() {
            return transactionMessage;
        }
};
#endif