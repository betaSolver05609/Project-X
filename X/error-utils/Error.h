
#include <string>
using namespace std;

class Error {
    private: 
        std::string errorMessage;
        int status;

    public :
        Error(std::string errorMessage, int status);
        void setError(std::string errorMessage, int status);
        std::string getErrorMessage();
        int getStatus();
};

Error::Error(std::string errorMessage, int status) {
    setError(errorMessage, status);
}

std::string Error::getErrorMessage() {
    return errorMessage;
}

int Error::getStatus() {
    return status;
}