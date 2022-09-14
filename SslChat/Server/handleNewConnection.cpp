// standard headers
#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// installed headers
#include <fmt/core.h>

// project headers
#include "client.hpp"
#include "handleNewConnection.hpp"
#include "logging.hpp"
#include "ssl.hpp"
#include "types.hpp"

int handleNewConnection(int                      master_socket,
                        sockaddr_in             &address,
                        int                     &addrlen,
                        SslChat_Ctx             &ctx,
                        std::map<int, Client *> &clients,
                        int                      max_clients,
                        int                      client_socket[])
{
    std::string welcomeMessage = "Welcome!";
    int         new_socket;

    if ((new_socket = accept(master_socket,
                             (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        LOG_ERROR("accept");
        return -1;
    }

    SslChat_Ssl *ssl = SslLib_new(ctx);
    SslLib_setFd(*ssl, new_socket);
    clients.insert(
        std::pair<int, Client *>(new_socket, new Client(ssl, new_socket)));

    LOG_INFO("Accepting SSL...");
    if (SslLib_accept(*ssl) <= 0)
    {
        LOG_INFO("Problem!");
        return -1;
    }
    else
    {
        LOG_INFO("New client!");
        if (SslLib_write(
                *ssl, welcomeMessage.c_str(), welcomeMessage.length()) < 0)
        {
            LOG_ERROR("Client failed sending!");
            return -1;
        }
    }

    // inform user of socket number - used in send and receive
    // commands
    LOG_INFO(fmt::format("New connection, socket fd is {}, ip is: {}, port: {}",
                         new_socket,
                         inet_ntoa(address.sin_addr),
                         ntohs(address.sin_port)));

    // add new socket to array of sockets
    for (int i = 0; i < max_clients; i++)
    {
        // if position is empty
        if (client_socket[i] == 0)
        {
            client_socket[i] = new_socket;
            LOG_INFO(
                fmt::format("Adding to list of sockets at "
                            "client_sockets position {}",
                            i));
            break;
        }
    }
    return new_socket;
}