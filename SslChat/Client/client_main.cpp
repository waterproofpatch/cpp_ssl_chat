#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <resolv.h>
#include <string.h>
#include <unistd.h>

#include <fmt/core.h>

#include "logging.hpp"

// Not sure what headers are needed or not
// This code (theoretically) writes "Hello World, 123" to a socket over a secure
// TLS connection compiled with g++ -Wall -o client.out client.cpp -L/usr/lib
// -lssl -lcrypto Based off of:
// https://www.cs.utah.edu/~swalton/listings/articles/ssl_client.c Some of the
// code was taken from this post:
// https://stackoverflow.com/questions/52727565/client-in-c-use-gethostbyname-or-getaddrinfo

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

SSL_CTX *InitSSL_CTX(void)
{
    const SSL_METHOD *method =
        TLS_client_method(); /* Create new client-method instance */
    SSL_CTX *ctx = SSL_CTX_new(method);

    if (ctx == nullptr)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
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

void DisplayCerts(SSL *ssl)
{
    X509 *cert =
        SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert != nullptr)
    {
        printf("Server certificates:\n");
        char *line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        delete line;
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        delete line;
        X509_free(cert);
    }
    else
    {
        printf("Info: No client certificates configured.\n");
    }
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

    SSL_CTX *ctx = InitSSL_CTX();
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

    printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
    DisplayCerts(ssl);
    const char *chars = "Hello World, 123!";
    SSL_write(ssl, chars, strlen(chars));
    SSL_free(ssl);
    close(sfd);
    SSL_CTX_free(ctx);
    return 0;
}