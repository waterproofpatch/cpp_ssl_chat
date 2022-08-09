
#ifndef _LOGGING_H
#define _LOGGING_H

#include <string>

typedef enum LogLevel
{
    LOG_INFO,
    LOG_DEBUG,
    LOG_WARN,
    LOG_ERROR
} eLogLevel;

void print_usage(void);

void logInfo(std::string message);
void logWarn(std::string message);
void logError(std::string message);
void logDebug(std::string message);

#endif