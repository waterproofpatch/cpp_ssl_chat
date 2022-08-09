#include <iostream>
#include <string>

#include "helpers.hpp"
#include "logging.hpp"

void print_usage(void)
{
    std::cout << "Usage: " << std::endl;
    std::cout << "./Server <path-to-cert.pem> <path-to-key.pem>" << std::endl;
}

void logInfo(std::string message)
{
    log(LOG_INFO, message);
}
void logWarn(std::string message)
{
    log(LOG_WARN, message);
}
void logError(std::string message)
{
    log(LOG_ERROR, message);
}
void logDebug(std::string message)
{
    log(LOG_DEBUG, message);
}
