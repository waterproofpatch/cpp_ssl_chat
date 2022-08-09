
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
void log(eLogLevel level, std::string message);
#endif