#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <resolv.h>
#include <string>
#include <unistd.h>

#include <fmt/core.h>

#include "constants.hpp"
#include "logging.hpp"
#include "openConnection.hpp"

int openConnection(std::string hostname, std::string port)
{
    struct hostent  *host;
    int              fd     = 0;
    int              err    = 0;
    struct addrinfo  hints  = {0};
    struct addrinfo *addrs  = NULL;
    int              status = 0;

    if ((host = gethostbyname(hostname.c_str())) == nullptr)
    {
        LOG_ERROR(hostname);
        exit(EXIT_FAILURE);
    }

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    status = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &addrs);
    if (status != 0)
    {
        LOG_ERROR(fmt::format("{}: {}", hostname, gai_strerror(status)));
        exit(EXIT_FAILURE);
    }

    for (struct addrinfo *addr = addrs; addr != nullptr; addr = addr->ai_next)
    {
        fd = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
        if (fd == ERROR_STATUS)
        {
            err = errno;
            continue;
        }

        LOG_INFO("Calling connect...");
        if (connect(fd, addr->ai_addr, addr->ai_addrlen) == 0)
        {
            break;
        }

        err = errno;
        fd  = ERROR_STATUS;
        close(fd);
    }

    freeaddrinfo(addrs);
    addrs = NULL;

    if (fd == ERROR_STATUS)
    {
        LOG_ERROR(fmt::format("{}: {}", hostname, strerror(err)));
        exit(EXIT_FAILURE);
    }
    return fd;
}