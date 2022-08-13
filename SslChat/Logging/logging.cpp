#include <iostream>
#include <string>

#include "helpers.hpp"
#include "logging.hpp"

void logInfo(std::string message, bool newline)
{
    log(LOG_INFO, message, newline);
}
void logWarn(std::string message, bool newline)
{
    log(LOG_WARN, message, newline);
}
void logError(std::string message, bool newline)
{
    log(LOG_ERROR, message, newline);
}
void logDebug(std::string message, bool newline)
{
    log(LOG_DEBUG, message, newline);
}
