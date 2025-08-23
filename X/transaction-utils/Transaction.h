#pragma once
#include <string>

class Transaction {
private:
    std::string status;  
    std::string message;

public:
    
    Transaction(const std::string& s, const std::string& m)
        : status(s), message(m) {}

    
    Transaction(bool success, const std::string& m) {
        status = success ? "SUCCESS" : "FAILURE";
        message = m;
    }

    std::string getStatus() const { return status; }
    std::string getMessage() const { return message; }
};
