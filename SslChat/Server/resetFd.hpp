#ifndef RESETFD_HPP
#define RESETFD_HPP

// standard headers
#include <sys/time.h>   //FD_SET, FD_ISSET, FD_ZERO macros

int resetFd(int     master_socket,
            fd_set &readfds,
            int     max_clients,
            int     client_socket[]);
#endif