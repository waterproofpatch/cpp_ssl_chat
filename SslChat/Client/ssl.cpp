#include <openssl/err.h>
#include <openssl/ssl.h>

#include <fmt/core.h>

#include "logging.hpp"

/**
 * @brief initialize an SSL context.
 *
 * @return SSL_CTX* a new SSL context.
 */
SSL_CTX *SslLib_initCtx(void)
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