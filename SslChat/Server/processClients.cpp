// standard headers
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <signal.h>
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
#include "cliLoop.hpp"
#include "client.hpp"
#include "handleNewConnection.hpp"
#include "initServer.hpp"
#include "logging.hpp"
#include "resetFd.hpp"
#include "ssl.hpp"

int serverSocket = 0;

static int pfd[2]; /* File descriptors for pipe */

static int max(int a, int b)
{
    return a > b ? a : b;
}

static void selfPipe()
{
    if (write(pfd[1], "x", 1) == -1 && errno != EAGAIN)
    {
        LOG_ERROR("Failed writing to pipe");
        return;
    }
}

static void handler(int sig)
{
    int savedErrno; /* In case we change 'errno' */
    selfPipe();
    savedErrno = errno;
    errno      = savedErrno;
}

static void cliLoopMessageHandler(std::string message, void *args)
{
    LOG_DEBUG(fmt::format("Handling message {}", message));
    if (message.compare("quit") == 0)
    {
        selfPipe();
    }
}

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
    struct sigaction        sa;

    serverSocket = SslLib_createSocket(port);
    ctx          = SslLib_getContext();
    SslLib_configureContext(ctx, certPath.c_str(), keyPath.c_str());
    std::thread cliThread(cliLoop, cliLoopMessageHandler, nullptr);

    if (initServer(
            serverSocket, max_clients, client_socket, address, port, addrlen) <
        0)
    {
        LOG_ERROR("Failed initing server!");
        return -1;
    }

    /* Create pipe before establishing signal handler to prevent race */

    if (pipe(pfd) == -1)
    {
        LOG_ERROR("Failed creating pipe!");
        return -1;
    }

    LOG_INFO("Waiting for connections ...");
    while (1)
    {
        int max_sd = resetFd(serverSocket, readfds, max_clients, client_socket);

        FD_SET(pfd[0], &readfds); /* Add read end of pipe to 'readfds' */
        max_sd =
            max(max_sd + 1, pfd[0] + 1); /* And adjust 'max_sd' if required */

        /* Make read and write ends of pipe nonblocking */

        int flags = fcntl(pfd[0], F_GETFL);
        if (flags == -1)
        {
            LOG_ERROR("fcntl-F_GETFL");
            return -1;
        }
        flags |= O_NONBLOCK; /* Make read end nonblocking */
        if (fcntl(pfd[0], F_SETFL, flags) == -1)
        {
            LOG_ERROR("fcntl-F_GETFL");
            return -1;
        }

        flags = fcntl(pfd[1], F_GETFL);
        if (flags == -1)
        {
            LOG_ERROR("fcntl-F_GETFL");
            return -1;
        }
        flags |= O_NONBLOCK; /* Make write end nonblocking */
        if (fcntl(pfd[1], F_SETFL, flags) == -1)
        {
            LOG_ERROR("fcntl-F_SETFL");
            return -1;
        }

        sigemptyset(&sa.sa_mask);
        sa.sa_flags   = SA_RESTART; /* Restart interrupted reads()s */
        sa.sa_handler = handler;
        if (sigaction(SIGINT, &sa, NULL) == -1)
        {
            LOG_ERROR("sigaction");
            return -1;
        }

        // wait for an activity on one of the sockets , timeout is NULL ,
        // so wait indefinitely
        int activity = 0;
        while ((activity = select(max_sd, &readfds, NULL, NULL, 0)) == -1 &&
               errno == EINTR)
            continue;       /* Restart if interrupted by signal */
        if (activity == -1) /* Unexpected error */
        {
            LOG_ERROR("Error on select!");
            return -1;
        }
        if (FD_ISSET(pfd[0], &readfds))
        {
            /* Handler was called */
            LOG_INFO("A signal was caught!");

            for (;;)
            {
                /* Consume bytes from pipe */
                int ch;
                if (read(pfd[0], &ch, 1) == -1)
                {
                    if (errno == EAGAIN)
                        break; /* No more bytes */
                    else
                        LOG_ERROR("read"); /* Some other error */
                }

                /* Perform any actions that should be taken in response to
                 * signal */
                LOG_INFO("Exiting!");
                goto fail;
            }
        }
        // activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            LOG_INFO("select error");
            goto fail;
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

fail:
done:
    LOG_DEBUG("Joining cliThread...");
    cliThread.join();
    LOG_DEBUG("Joined thread.");
    return 0;
}