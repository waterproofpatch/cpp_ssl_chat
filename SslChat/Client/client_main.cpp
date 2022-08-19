#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <resolv.h>
#include <string.h>
#include <thread>
#include <unistd.h>

#include <fmt/core.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "logging.hpp"
#include "ssl.hpp"

const int ERROR_STATUS = -1;

typedef struct FarEnd
{
    std::string ip;
    std::string port;
} tFarEnd;

void print_usage(void)
{
    std::cout << "Usage: " << std::endl;
    std::cout << "./Client <ip> <port>" << std::endl;
}

int OpenConnection(std::string hostname, std::string port)
{
    struct hostent *host;
    if ((host = gethostbyname(hostname.c_str())) == nullptr)
    {
        LOG_ERROR(hostname);
        exit(EXIT_FAILURE);
    }

    struct addrinfo hints = {0}, *addrs;
    hints.ai_family       = AF_UNSPEC;
    hints.ai_socktype     = SOCK_STREAM;
    hints.ai_protocol     = IPPROTO_TCP;

    const int status =
        getaddrinfo(hostname.c_str(), port.c_str(), &hints, &addrs);
    if (status != 0)
    {
        LOG_ERROR(fmt::format("{}: {}", hostname, gai_strerror(status)));
        exit(EXIT_FAILURE);
    }

    int sfd, err;
    for (struct addrinfo *addr = addrs; addr != nullptr; addr = addr->ai_next)
    {
        sfd = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
        if (sfd == ERROR_STATUS)
        {
            err = errno;
            continue;
        }

        LOG_INFO("Calling connect...");
        if (connect(sfd, addr->ai_addr, addr->ai_addrlen) == 0)
        {
            break;
        }

        err = errno;
        sfd = ERROR_STATUS;
        close(sfd);
    }

    freeaddrinfo(addrs);

    if (sfd == ERROR_STATUS)
    {
        LOG_ERROR(fmt::format("{}: {}", hostname, strerror(err)));
        exit(EXIT_FAILURE);
    }
    return sfd;
}

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
int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        print_usage();
        return 1;
    }

    tFarEnd farEnd = {std::string(argv[1]), std::string(argv[2])};
    LOG_INFO(fmt::format("Connecting to {}:{}", argv[1], argv[2]));

    SSL_CTX *ctx = SslLib_initCtx();
    SSL     *ssl = SSL_new(ctx);
    if (ssl == nullptr)
    {
        LOG_ERROR("SSL_new() failed");
        exit(EXIT_FAILURE);
    }

    // Host is hardcoded to localhost for testing purposes
    LOG_INFO("OpenConnection...");
    const int sfd = OpenConnection(farEnd.ip, farEnd.port);
    SSL_set_fd(ssl, sfd);

    LOG_INFO("SSL_connect...");
    const int status = SSL_connect(ssl);
    if (status != 1)
    {
        SSL_get_error(ssl, status);
        ERR_print_errors_fp(stderr);   // High probability this doesn't do
                                       // anything
        LOG_ERROR(fmt::format("SSL_connect failed with SSL_get_error code {:d}",
                              status));
        exit(EXIT_FAILURE);
    }

    LOG_INFO(fmt::format("Connected with {} encryption", SSL_get_cipher(ssl)));
    SslLib_displayCerts(ssl);

    auto readMessageThread = std::thread(readMessages, ssl);
    if (handleMessages(ssl) < 0)
    {
        LOG_ERROR("handleMessages returned an error!");
    }

    close(sfd);
    LOG_INFO("Waiting for readMessageThread to join...");
    readMessageThread.join();
    LOG_INFO("readMessageThread joined.");
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    return 0;
}