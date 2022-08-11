
#ifndef _LOGGING_H
#define _LOGGING_H

#include <string>

#define LOG_INFO(msg) \
    logInfo(fmt::format("{}:{}: {}", __FILE_NAME__, __LINE__, msg))
#define LOG_ERROR(msg) \
    logError(fmt::format("{}:{}: {}", __FILE_NAME__, __LINE__, msg))

void logInfo(std::string message);
void logWarn(std::string message);
void logError(std::string message);
void logDebug(std::string message);

#endif