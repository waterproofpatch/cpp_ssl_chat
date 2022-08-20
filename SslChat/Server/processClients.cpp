// standard headers
#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>   //FD_SET, FD_ISSET, FD_ZERO macros
#include <thread>
#include <unistd.h>

// installed headers
#include <fmt/core.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

// project headers
#include "client.hpp"
#include "handleNewConnection.hpp"
#include "initServer.hpp"
#include "logging.hpp"
#include "processCliThread.hpp"
#include "resetFd.hpp"
#include "ssl.hpp"

int serverSocket = 0;

int processClients(std::string    certPath,
                   std::string    keyPath,
                   unsigned short port)
{
    std::map<int, Client *> clients;
    SSL_CTX                *ctx = NULL;
    struct sockaddr_in      address;
    int                     addrlen    = 0;
    int                     new_socket = 0;
    int                     client_socket[30];
    int                     max_clients  = 30;
    int                     activity     = 0;
    int                     i            = 0;
    int                     valread      = 0;
    char                    buffer[1025] = {0};   // data buffer of 1K
    fd_set                  readfds;
    std::thread             cliThread;

    serverSocket = SslLib_createSocket(port);
    ctx          = SslLib_getContext();
    SslLib_configureContext(ctx, certPath.c_str(), keyPath.c_str());
    cliThread = std::thread(processCliThread);

    if (initServer(
            serverSocket, max_clients, client_socket, address, port, addrlen) <
        0)
    {
        LOG_ERROR("Failed initing server!");
        return -1;
    }

    LOG_INFO("Waiting for connections ...");
    while (1)
    {
        int max_sd = resetFd(serverSocket, readfds, max_clients, client_socket);

        // wait for an activity on one of the sockets , timeout is NULL ,
        // so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            LOG_INFO("select error");
            return -1;
        }

        // If something happened on the master socket ,
        // then its an incoming connection
        if (FD_ISSET(serverSocket, &readfds))
        {
            handleNewConnection(serverSocket,
                                address,
                                addrlen,
                                ctx,
                                clients,
                                max_clients,
                                client_socket);
        }

        // else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            int sd = client_socket[i];

            if (FD_ISSET(sd, &readfds))
            {
                // Check if it was for closing, and also read the
                // incoming message
                memset(buffer, 0, sizeof(buffer));
                if ((valread =
                         SSL_read(clients.at(sd)->getSsl(), buffer, 1024)) == 0)
                {
                    // Somebody disconnected , get his details and print
                    getpeername(
                        sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    LOG_INFO(
                        fmt::format("Client {} disconnected, ip {} , port {}",
                                    sd,
                                    inet_ntoa(address.sin_addr),
                                    ntohs(address.sin_port)));

                    // Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;
                    clients.erase(sd);
                }

                // Echo back the message that came in
                else
                {
                    // set the string terminating NULL byte on the end
                    // of the data read
                    buffer[valread] = '\0';
                    LOG_INFO(fmt::format("Client {} sent [{}]", sd, buffer));
                    SSL_write(clients.at(sd)->getSsl(), buffer, strlen(buffer));
                }
            }
        }
    }
    return 0;
}