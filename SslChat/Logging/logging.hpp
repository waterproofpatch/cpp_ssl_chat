
#ifndef _LOGGING_H
#define _LOGGING_H

#include <string>

#define LOG_PROMPT() logInfo(fmt::format("> "), false)
#define LOG_INFO(msg) \
    logInfo(fmt::format("{}:{}: {}", __FILE_NAME__, __LINE__, msg), true)
#define LOG_ERROR(msg) \
    logError(fmt::format("{}:{}: {}", __FILE_NAME__, __LINE__, msg), true)

void logInfo(std::string message, bool newline);
void logWarn(std::string message, bool newline);
void logError(std::string message, bool newline);
void logDebug(std::string message, bool newline);

#endif