#ifndef SSL_HPP
#define SSL_HPP

#include <openssl/err.h>
#include <openssl/ssl.h>

SSL_CTX *SslLib_initCtx(void);
void     SslLib_displayCerts(SSL *ssl);

#endif