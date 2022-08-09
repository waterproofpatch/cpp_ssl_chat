#include <iostream>
#include <string>

#include "logging.hpp"

void print_usage(void)
{
    std::cout << "Usage: " << std::endl;
    std::cout << "./Server <path-to-cert.pem> <path-to-key.pem>" << std::endl;
}

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