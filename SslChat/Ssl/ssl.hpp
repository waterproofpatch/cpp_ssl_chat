#ifndef SSL_HPP
#define SSL_HPP

#include "types.hpp"

SslChat_Ctx *SslLib_getClientContext(void);
SslChat_Ctx *SslLib_getServerContext();
void         SslLib_displayCerts(SSL *ssl);
int          SslLib_createSocket(int port);
void         SslLib_configureContext(SslChat_Ctx *ctx,
                                     const char  *certPath,
                                     const char  *keyPath);

#endif