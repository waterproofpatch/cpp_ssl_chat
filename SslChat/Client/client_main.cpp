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

#include "constants.hpp"
#include "handleMessages.hpp"
#include "logging.hpp"
#include "openConnection.hpp"
#include "readMessages.hpp"
#include "ssl.hpp"
#include "types.hpp"

static void printUsage(void)
{
    std::cout << "Usage: " << std::endl;
    std::cout << "./Client <ip> <port>" << std::endl;
}

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        printUsage();
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
    LOG_INFO("openConnection...");
    const int sfd = openConnection(farEnd.ip, farEnd.port);
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

    LOG_INFO("Closing socket...");

    // send a canary to cause the thread to close
    SSL_write(ssl, "aaaaa", 1);

    close(sfd);

    LOG_INFO("Waiting for readMessageThread to join...");
    readMessageThread.join();
    LOG_INFO("readMessageThread joined.");
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    return 0;
}