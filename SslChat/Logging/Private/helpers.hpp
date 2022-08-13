#ifndef _HELPERS_H
#define _HELPERS_H

#include <string>

typedef enum LogLevel
{
    LOG_INFO,
    LOG_DEBUG,
    LOG_WARN,
    LOG_ERROR
} eLogLevel;

void log(eLogLevel level, std::string message, bool newline);

#endif