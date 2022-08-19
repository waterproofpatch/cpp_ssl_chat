#ifndef INITSERVER_HPP
#define INITSERVER_HPP
// standard headers
#include <arpa/inet.h>

int initServer(int                 socket,
               int                 max_clients,
               int                 client_socket[],
               struct sockaddr_in &address,
               unsigned short      port,
               int                &addrlen);
#endif