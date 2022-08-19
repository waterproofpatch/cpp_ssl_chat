
// standard headers
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>   //FD_SET, FD_ISSET, FD_ZERO macros
#include <unistd.h>

// project headers
#include "client.hpp"
#include "resetFd.hpp"
#include "ssl.hpp"

int resetFd(int     master_socket,
            fd_set &readfds,
            int     max_clients,
            int     client_socket[])
{

    // clear the socket set
    FD_ZERO(&readfds);

    // add master socket to set
    FD_SET(master_socket, &readfds);
    int max_sd = master_socket;

    // add child sockets to set
    for (int i = 0; i < max_clients; i++)
    {
        // socket descriptor
        int sd = client_socket[i];

        // if valid socket descriptor then add to read list
        if (sd > 0)
        {
            FD_SET(sd, &readfds);
        }

        // highest file descriptor number, need it for the select
        // function
        if (sd > max_sd)
        {
            max_sd = sd;
        }
    }
    return max_sd;
}
