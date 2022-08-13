#include <iostream>

#include "helpers.hpp"

void log(eLogLevel level, std::string message, bool newline)
{
    switch (level)
    {
        case LOG_INFO:
            std::cout << "INFO: " << message;
            break;
        case LOG_DEBUG:
            std::cout << "DEBUG: " << message;
            break;
        case LOG_ERROR:
            std::cerr << "ERROR: " << message;
            break;
        case LOG_WARN:
            std::cout << "WARN: " << message;
            break;
    }
    if (newline)
    {
        std::cout << std::endl;
    }
}