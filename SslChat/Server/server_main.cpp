#include <iostream>
#include <thread>
#include <vector>

#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <openssl/err.h>
#include <openssl/ssl.h>

#include <fmt/core.h>

#include "logging.hpp"
#include "ssl.hpp"

class Client
{
    int          socket;
    SSL         *ssl;
    std::thread *t;

   public:
    void start()
    {
        LOG_INFO("Starting thread...");
        this->t = new std::thread(&Client::run, this);
        LOG_INFO("Started thread...");
    }
    void stop()
    {
        LOG_INFO("Joining thread...");
        close(this->socket);
        this->t->join();
        LOG_INFO("Joined thread...");
    }
    void run()
    {
        LOG_INFO("Running!");
        SSL_write(this->ssl, "Reply!", strlen("Reply!"));
        return;
    }
    Client() = delete;
    Client(SSL *ssl, int socket)
    {
        LOG_INFO("Client constructing!");
        this->ssl    = ssl;
        this->socket = socket;
    }
    ~Client()
    {
        LOG_INFO("Client destructing!");
        SSL_shutdown(this->ssl);
        SSL_free(this->ssl);
    }

   private:
};

void print_usage(void)
{
    std::cout << "Usage: " << std::endl;
    std::cout << "./Server <path-to-cert.pem> <path-to-key.pem>" << std::endl;
}

int startServer(std::string certPath, std::string keyPath, unsigned short port)
{
    std::vector<Client *> clients;
    int                   sock = 0;
    SSL_CTX              *ctx  = NULL;

    sock = SslLib_createSocket(port);
    ctx  = SslLib_getContext();
    SslLib_configureContext(ctx, certPath.c_str(), keyPath.c_str());

    LOG_INFO("Entering loop...");

    /* Handle connections */
    while (1)
    {
        struct sockaddr_in addr;
        unsigned int       len = sizeof(addr);
        SSL               *ssl;
        const char         reply[] = "test\n";

        LOG_INFO("Waiting for client...");
        int clientSockFd = accept(sock, (struct sockaddr *)&addr, &len);
        if (clientSockFd < 0)
        {
            LOG_ERROR("Unable to accept");
            exit(EXIT_FAILURE);
        }
        LOG_INFO(
            fmt::format("Received clientSockFd on socket {}", clientSockFd));

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, clientSockFd);

        LOG_INFO("Accepting SSL...");
        if (SSL_accept(ssl) <= 0)
        {
            LOG_INFO("Problem!");
            ERR_print_errors_fp(stderr);
        }
        else
        {
            LOG_INFO("Creating client!");
            Client *client = new Client(ssl, clientSockFd);
            clients.push_back(client);
            client->start();
        }
    }

    for (auto &c : clients)   // access by reference to avoid copying
    {
        LOG_INFO("Stopping client...");
        c->stop();
    }

    close(sock);
    SSL_CTX_free(ctx);
}

int main(int argc, char **argv)
{

    if (argc < 3)
    {
        print_usage();
        return 1;
    }

    return startServer(argv[1], argv[2], 5000);
}