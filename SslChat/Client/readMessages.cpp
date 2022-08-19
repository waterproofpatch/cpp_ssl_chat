#include <fmt/core.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "logging.hpp"
#include "readMessages.hpp"

void readMessages(SSL *ssl)
{
    while (1)
    {
        char buffer[1024] = {0};
        int  numRead      = SSL_read(ssl, buffer, sizeof(buffer));
        if (numRead < 0)
        {
            LOG_INFO("Server closed connection!");
            break;
        }
        LOG_INFO(fmt::format("Read [{}]", buffer));
    }
    LOG_INFO("Tearing down!");
}