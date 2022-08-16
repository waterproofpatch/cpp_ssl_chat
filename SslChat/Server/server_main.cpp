// standard headers
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>   //FD_SET, FD_ISSET, FD_ZERO macros
#include <thread>
#include <unistd.h>
#include <vector>

// installed headers
#include <fmt/core.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

// project headers
#include "client.hpp"
#include "logging.hpp"
#include "safe_queue.hpp"
#include "ssl.hpp"

#define TRUE 1
#define FALSE 0

void print_usage(void)
{
    std::cout << "Usage: " << std::endl;
    std::cout << "./Server <path-to-cert.pem> <path-to-key.pem>" << std::endl;
}

/**
 * @brief handle the CLI
 *
 * @param sock the master socket. Closed when 'quit' is entered at the CLI.
 */
void processCliThread(int serverSocket)
{
    LOG_PROMPT();
    for (std::string line; std::getline(std::cin, line);)
    {
        std::cout << line << std::endl;
        if (line.compare("quit") == 0)
        {
            LOG_INFO("Quitting.");
            close(serverSocket);
            return;
        }
        LOG_PROMPT();
    }
}

Client *getClientBySocket(std::vector<Client *> &clients, int socket)
{
    for (auto &client : clients)
    {
        if (client->getSocket() == socket)
        {
            return client;
        }
    }
    return NULL;
}

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

int handleNewConnection(int                    master_socket,
                        sockaddr_in           &address,
                        int                   &addrlen,
                        SSL_CTX               *ctx,
                        std::vector<Client *> &clients,
                        int                    max_clients,
                        int                    client_socket[])
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

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, new_socket);
    clients.push_back(new Client(ssl, new_socket));

    LOG_INFO("Accepting SSL...");
    if (SSL_accept(ssl) <= 0)
    {
        LOG_INFO("Problem!");
        return -1;
    }
    else
    {
        LOG_INFO("New client!");
        if (SSL_write(ssl, welcomeMessage.c_str(), welcomeMessage.length()) < 0)
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

int processClients(std::string    certPath,
                   std::string    keyPath,
                   unsigned short port)
{
    std::vector<Client *> clients;
    int                   master_socket = 0;
    SSL_CTX              *ctx           = NULL;
    struct sockaddr_in    address;
    int                   addrlen    = 0;
    int                   new_socket = 0;
    int                   client_socket[30];
    int                   max_clients  = 30;
    int                   activity     = 0;
    int                   i            = 0;
    int                   valread      = 0;
    char                  buffer[1025] = {0};   // data buffer of 1K
    fd_set                readfds;
    std::thread           cliThread;

    master_socket = SslLib_createSocket(port);
    ctx           = SslLib_getContext();
    SslLib_configureContext(ctx, certPath.c_str(), keyPath.c_str());
    cliThread = std::thread(processCliThread, master_socket);

    if (initServer(
            master_socket, max_clients, client_socket, address, port, addrlen) <
        0)
    {
        LOG_ERROR("Failed initing server!");
        return -1;
    }

    LOG_INFO("Waiting for connections ...");
    while (TRUE)
    {
        int max_sd =
            resetFd(master_socket, readfds, max_clients, client_socket);

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
        if (FD_ISSET(master_socket, &readfds))
        {
            handleNewConnection(master_socket,
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
                // Check if it was for closing , and also read the
                // incoming message
                memset(buffer, 0, sizeof(buffer));
                if ((valread =
                         SSL_read(getClientBySocket(clients, sd)->getSsl(),
                                  buffer,
                                  1024)) == 0)
                {
                    // Somebody disconnected , get his details and print
                    getpeername(
                        sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    LOG_INFO(fmt::format("Host disconnected , ip {} , port {}",
                                         inet_ntoa(address.sin_addr),
                                         ntohs(address.sin_port)));

                    // Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;
                }

                // Echo back the message that came in
                else
                {
                    // set the string terminating NULL byte on the end
                    // of the data read
                    buffer[valread] = '\0';
                    SSL_write(getClientBySocket(clients, sd)->getSsl(),
                              buffer,
                              strlen(buffer));
                }
            }
        }
    }
    return 0;
}

int main(int argc, char **argv)
{

    if (argc < 3)
    {
        print_usage();
        return 1;
    }

    return processClients(argv[1], argv[2], 5000);
}