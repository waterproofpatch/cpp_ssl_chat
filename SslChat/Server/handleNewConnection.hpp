#ifndef HANDLENEWCONNECTION_HPP
#define HANDLENEWCONNECTION_HPP

// standard headers
#include <arpa/inet.h>
#include <iostream>
#include <map>

// installed headers
#include <openssl/ssl.h>

// project headers
#include "client.hpp"

int handleNewConnection(int                      master_socket,
                        sockaddr_in             &address,
                        int                     &addrlen,
                        SSL_CTX                 *ctx,
                        std::map<int, Client *> &clients,
                        int                      max_clients,
                        int                      client_socket[]);

#endif