#include <iostream>
#include "logger.h"

namespace logger 
{
    void init()
    {
        std::ios::sync_with_stdio(false);
    }

    void info(const std::string& msg)
    {
        std::cout << "[INFO] " << msg << std::endl;
    }

    void warn(const std::string& msg)
    {
        std::cerr << "[WARNING] " << msg << std::endl;
    }

    void error(const std::string& msg)
    {
        std::cerr << "[ERROR] " << msg << std::endl;
    }
}