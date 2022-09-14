#ifndef SSL_HPP
#define SSL_HPP

#include "types.hpp"
#include <iostream>

SslChat_Ctx        *SslLib_getClientContext(void);
SslChat_Ctx        *SslLib_getServerContext();
void                SslLib_displayCerts(SSL *ssl);
int                 SslLib_createSocket(int port);
void                SslLib_configureContext(SslChat_Ctx *ctx,
                                            const char  *certPath,
                                            const char  *keyPath);
tSslChat_SslHandle *SslLib_getServerHandle(std::string certPath,
                                           std::string keyPath);
int SslLib_read(tSslChat_SslHandle &handle, unsigned char *buf, size_t length);
int SslLib_write(tSslChat_SslHandle &handle, unsigned char *buf, size_t length);
SslChat_Ssl *SslLib_new(SslChat_Ctx &ctx);
void         SslLib_setFd(SslChat_Ssl &ssl, int socket);
int          SslLib_accept(SslChat_Ssl &ssl);
int          SslLib_write(SslChat_Ssl &ssl, const char *payload, size_t length);

#endif