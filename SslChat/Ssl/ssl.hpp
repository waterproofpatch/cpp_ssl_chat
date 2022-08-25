#ifndef SSL_HPP
#define SSL_HPP

#include <openssl/ssl.h>

SSL_CTX *SslLib_initCtx(void);
void     SslLib_displayCerts(SSL *ssl);
int      SslLib_createSocket(int port);
SSL_CTX *SslLib_getContext();
void     SslLib_configureContext(SSL_CTX    *ctx,
                                 const char *certPath,
                                 const char *keyPath);

#endif