#include <iostream>
#include <string>

#include <fmt/core.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "handleMessages.hpp"
#include "logging.hpp"

int handleMessages(SSL *ssl)
{
    for (std::string line; std::getline(std::cin, line);)
    {
        std::cout << line << std::endl;
        if (line.compare("quit") == 0)
        {
            LOG_INFO("Quitting.");
            break;
        }
        LOG_INFO(fmt::format("Sending [{}]", line));
        SSL_write(ssl, line.c_str(), line.length());
        LOG_PROMPT();
    }
    return 0;
}