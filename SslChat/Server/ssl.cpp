
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <openssl/err.h>
#include <openssl/ssl.h>

#include <fmt/core.h>

#include "logging.hpp"

static int SslLib_setPasswordCallback(char *buf, int size, int rwflag, void *u)
{
    strncpy(buf, (char *)u, size);
    buf[size - 1] = '\0';
    return strlen(buf);
}

int SslLib_createSocket(int port)
{
    int                s;
    struct sockaddr_in addr;

    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        LOG_ERROR("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        LOG_ERROR("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(s, 1) < 0)
    {
        LOG_ERROR("Unable to listen");
        exit(EXIT_FAILURE);
    }

    return s;
}

SSL_CTX *SslLib_getContext()
{
    const SSL_METHOD *method;
    SSL_CTX          *ctx;

    method = TLS_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        LOG_ERROR("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void SslLib_configureContext(SSL_CTX    *ctx,
                             const char *certPath,
                             const char *keyPath)
{
    SSL_CTX_set_default_passwd_cb_userdata(ctx, (void *)"test");
    SSL_CTX_set_default_passwd_cb(ctx, SslLib_setPasswordCallback);
    LOG_INFO("OK!");

    /* Set the key and cert */
    LOG_INFO(fmt::format("Loading {}!", certPath));
    if (SSL_CTX_use_certificate_file(ctx, certPath, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    LOG_INFO(fmt::format("Loading {}!", keyPath));
    if (SSL_CTX_use_PrivateKey_file(ctx, keyPath, SSL_FILETYPE_PEM) <= 0)
    {
        LOG_INFO("Some sort of problem");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    LOG_INFO("Leaving function");
}