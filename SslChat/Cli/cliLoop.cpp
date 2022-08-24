#include <iostream>
#include <string>

#include <fmt/core.h>

#include "cliLoop.hpp"
#include "logging.hpp"

int cliLoop(handleMessageCallback callback, void *args)
{
    for (std::string line; std::getline(std::cin, line);)
    {
        callback(line, args);
        if (line.compare("quit") == 0)
        {
            LOG_INFO("Quitting.");
            break;
        }
    }
    return 0;
}