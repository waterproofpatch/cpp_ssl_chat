// standard headers
#include <iostream>
#include <string>
#include <unistd.h>

// installed headers
#include <fmt/core.h>
#include <openssl/ssl.h>

// project heaers
#include "logging.hpp"
#include "processCliThread.hpp"

/**
 * @brief handle the CLI
 *
 */
void processCliThread()
{
    LOG_PROMPT();
    for (std::string line; std::getline(std::cin, line);)
    {
        std::cout << line << std::endl;
        if (line.compare("quit") == 0)
        {
            LOG_INFO("Quitting.");
            return;
        }
        LOG_PROMPT();
    }
}