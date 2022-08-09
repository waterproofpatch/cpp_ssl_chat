#include <iostream>

#include "helpers.hpp"

void log(eLogLevel level, std::string message)
{
    switch (level)
    {
        case LOG_INFO:
            std::cout << "INFO: " << message << std::endl;
            break;
        case LOG_DEBUG:
            std::cout << "DEBUG: " << message << std::endl;
            break;
        case LOG_ERROR:
            std::cerr << "ERROR: " << message << std::endl;
            break;
        case LOG_WARN:
            std::cout << "WARN: " << message << std::endl;
            break;
    }
}