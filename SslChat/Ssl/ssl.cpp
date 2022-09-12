#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <openssl/err.h>
#include <openssl/ssl.h>

#include <fmt/core.h>   // fmt::format

#include "logging.hpp"
#include "ssl.hpp"
#include "types.hpp"

static SSL_CTX *getNewCtx(const SSL_METHOD &method)
{
    SSL_CTX *ctx = SSL_CTX_new(&method);

    if (!ctx)
    {
        LOG_ERROR("Unable to create SSL context");
    }

    return ctx;
}

static int SslLib_setPasswordCallback(char *buf, int size, int rwflag, void *u)
{
    strncpy(buf, (char *)u, size);
    buf[size - 1] = '\0';
    return strlen(buf);
}

/**
 * @brief allocate a new SSL handle for listening, reading and writing.
 *
 * @param certPath path on disk to the certificate file.
 * @param keyPath path on disk to the key file.
 * @return tSslChat_SslHandle*
 */
tSslChat_SslHandle *SslLib_getServerHandle(std::string certPath,
                                           std::string keyPath)
{
    SslChat_Ctx *ctx = SslLib_getServerContext();
    SslLib_configureContext(ctx, certPath.c_str(), keyPath.c_str());
    SSL                *ssl    = SSL_new(ctx);
    tSslChat_SslHandle *handle = new tSslChat_SslHandle(ctx, ssl);
    return handle;
}

int SslLib_read(tSslChat_SslHandle &handle, unsigned char *buf, size_t length)
{
    return SSL_read(handle.ssl, buf, length);
}

int SslLib_write(tSslChat_SslHandle &handle, unsigned char *buf, size_t length)
{
    return SSL_write(handle.ssl, buf, length);
}

int SslLib_createSocket(int port)
{
    int                s;
    struct sockaddr_in addr;
    int                reuse = 1;

    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        LOG_ERROR("Unable to create socket");
        exit(EXIT_FAILURE);
    }
    setsockopt(
        s, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse));

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

/**
 * @brief initialize an SSL context for a server.
 *
 * @return SSL_CTX* a new SSL context.
 */
SslChat_Ctx *SslLib_getServerContext()
{
    const SSL_METHOD *method = TLS_server_method();
    return getNewCtx(*method);
}

/**
 * @brief initialize an SSL context for a client.
 *
 * @return SSL_CTX* a new SSL context.
 */
SslChat_Ctx *SslLib_getClientContext(void)
{
    const SSL_METHOD *method = TLS_client_method();
    return getNewCtx(*method);
}

void SslLib_configureContext(SslChat_Ctx *ctx,
                             const char  *certPath,
                             const char  *keyPath)
{
    SSL_CTX_set_default_passwd_cb_userdata(
        ctx, (void *)"test");   // TODO bring this in from the environment
    SSL_CTX_set_default_passwd_cb(ctx, SslLib_setPasswordCallback);

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

/**
 * @brief Display certificate information for this SSL context.
 *
 * @param ssl the SSL object.
 */
void SslLib_displayCerts(SSL *ssl)
{
    X509 *cert =
        SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert != nullptr)
    {
        LOG_INFO("Server certificates:");
        char *line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        LOG_INFO(fmt::format("Subject: {}", line));
        delete line;
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        LOG_INFO(fmt::format("Issuer: {}", line));
        delete line;
        X509_free(cert);
    }
    else
    {
        LOG_INFO("Info: No client certificates configured.\n");
    }
}