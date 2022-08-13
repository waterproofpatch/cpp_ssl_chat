// standard headers
#include <thread>
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

void Client::start()
{
    LOG_INFO("Starting thread...");
    this->t = new std::thread(&Client::run, this);
    LOG_INFO("Started thread...");
}

void Client::stop()
{
    LOG_INFO("Joining thread...");
    close(this->socket);
    this->t->join();
    LOG_INFO("Joined thread...");
}

void Client::run()
{
    LOG_INFO("Running!");
    SSL_write(this->ssl, "Reply!", strlen("Reply!"));
    return;
}
