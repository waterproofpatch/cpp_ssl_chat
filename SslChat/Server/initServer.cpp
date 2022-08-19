// standard headers
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// installed headers
#include <fmt/core.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "initServer.hpp"

// project headers
#include "logging.hpp"

int initServer(int                 socket,
               int                 max_clients,
               int                 client_socket[],
               struct sockaddr_in &address,
               unsigned short      port,
               int                &addrlen)
{
    // initialise all client_socket[] to 0 so not checked
    for (int i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    // type of socket created
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons(port);
    LOG_INFO(fmt::format("Listening on port {}", port));

    // try to specify maximum of 3 pending connections for the master socket
    if (listen(socket, 3) < 0)
    {
        LOG_ERROR("listen");
        return -1;
    }

    // accept the incoming connection
    addrlen = sizeof(address);
    return 0;
}