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
    struct hostent *host;
    if ((host = gethostbyname(hostname.c_str())) == nullptr)
    {
        LOG_ERROR(hostname);
        exit(EXIT_FAILURE);
    }

    struct addrinfo hints = {0}, *addrs;
    hints.ai_family       = AF_UNSPEC;
    hints.ai_socktype     = SOCK_STREAM;
    hints.ai_protocol     = IPPROTO_TCP;

    const int status =
        getaddrinfo(hostname.c_str(), port.c_str(), &hints, &addrs);
    if (status != 0)
    {
        LOG_ERROR(fmt::format("{}: {}", hostname, gai_strerror(status)));
        exit(EXIT_FAILURE);
    }

    int sfd, err;
    for (struct addrinfo *addr = addrs; addr != nullptr; addr = addr->ai_next)
    {
        sfd = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
        if (sfd == ERROR_STATUS)
        {
            err = errno;
            continue;
        }

        LOG_INFO("Calling connect...");
        if (connect(sfd, addr->ai_addr, addr->ai_addrlen) == 0)
        {
            break;
        }

        err = errno;
        sfd = ERROR_STATUS;
        close(sfd);
    }

    freeaddrinfo(addrs);

    if (sfd == ERROR_STATUS)
    {
        LOG_ERROR(fmt::format("{}: {}", hostname, strerror(err)));
        exit(EXIT_FAILURE);
    }
    return sfd;
}