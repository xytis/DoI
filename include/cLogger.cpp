
#include "cLogger.h"

void cLogger::warning(std::string msg)
{
    std::cout << "W>" << msg << std::endl;
}

void cLogger::error(std::string msg)
{
    std::cout << "E>" << msg << std::endl;
}
