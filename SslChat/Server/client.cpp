// standard headers
#include <unistd.h>

// installed headers
#include <fmt/core.h>
#include <openssl/ssl.h>

// project headers
#include "client.hpp"
#include "logging.hpp"

Client::Client(SSL *ssl, int socket)
{
    LOG_INFO("Client constructing!");
    this->ssl    = ssl;
    this->socket = socket;
    this->name   = fmt::format("client_{}", this->socket);
}

Client::~Client()
{

    LOG_INFO("Client destructing!");
    SSL_shutdown(this->ssl);
    SSL_free(this->ssl);
}

void Client::log(std::string msg)
{
    std::string me = std::string(*this);

    LOG_INFO(fmt::format("{}: {}", me, msg));
}

int Client::sendMessage(const char *message, size_t len)
{
    log("Handling message!");
    int numWritten = SSL_write(this->ssl, message, len);
    if (numWritten < 0)
    {
        this->log("Error!");
        return -1;
    }
    return 0;
}
