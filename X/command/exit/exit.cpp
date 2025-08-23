#include "exit.h"
#include <iostream>
#include "../../common_utils/common_utils.h"

bool handleExit(std::stringstream &ss, KeySpace &db)
{
    (void)ss; 
    saveDb(db);
    std::cout << "Exiting. Database saved.\n";
    return true;
}
