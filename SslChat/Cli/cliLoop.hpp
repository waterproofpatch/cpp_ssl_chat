#ifndef CLILOOP_H_
#define CLILOOP_H_

#include <iostream>

#include <openssl/ssl.h>

typedef void (*handleMessageCallback)(std::string, void *args);

int cliLoop(handleMessageCallback callback, void *args);

#endif