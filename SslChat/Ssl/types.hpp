#ifndef TYPES_H_
#define TYPES_H_

#include <openssl/ssl.h>

typedef SSL_CTX SslChat_Ctx;

typedef struct SslChat_SslHandle
{
    SslChat_Ctx *ctx;
    SSL         *ssl;
    SslChat_SslHandle() = delete;
    SslChat_SslHandle(SslChat_Ctx *ctx, SSL *ssl) : ctx(ctx), ssl(ssl){};
} tSslChat_SslHandle;

#endif