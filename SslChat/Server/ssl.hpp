
#include <openssl/ssl.h>

int      SslLib_createSocket(int port);
SSL_CTX *SslLib_getContext();
void     SslLib_configureContext(SSL_CTX    *ctx,
                                 const char *certPath,
                                 const char *keyPath);