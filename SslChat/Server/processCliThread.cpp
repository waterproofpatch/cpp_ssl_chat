// standard headers
#include <iostream>
#include <string>
#include <unistd.h>

// installed headers
#include <fmt/core.h>

// project heaers
#include "logging.hpp"
#include "processCliThread.hpp"

/**
 * @brief handle the CLI
 *
 * @param sock the master socket. Closed when 'quit' is entered at the CLI.
 */
void processCliThread(int serverSocket)
{
    LOG_PROMPT();
    for (std::string line; std::getline(std::cin, line);)
    {
        std::cout << line << std::endl;
        if (line.compare("quit") == 0)
        {
            LOG_INFO("Quitting.");
            close(serverSocket);
            return;
        }
        LOG_PROMPT();
    }
}