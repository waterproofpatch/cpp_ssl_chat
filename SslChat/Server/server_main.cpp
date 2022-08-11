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

int create_socket(int port)
{
    int                s;
    struct sockaddr_in addr;

    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        LOG_ERROR("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        LOG_ERROR("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(s, 1) < 0)
    {
        LOG_ERROR("Unable to listen");
        exit(EXIT_FAILURE);
    }

    return s;
}

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX          *ctx;

    method = TLS_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        LOG_ERROR("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

int ssl_set_password_callback(char *buf, int size, int rwflag, void *u)
{
    strncpy(buf, (char *)u, size);
    buf[size - 1] = '\0';
    return strlen(buf);
}

void configure_context(SSL_CTX *ctx, const char *certPath, const char *keyPath)
{
    SSL_CTX_set_default_passwd_cb_userdata(ctx, (void *)"test");
    SSL_CTX_set_default_passwd_cb(ctx, ssl_set_password_callback);
    LOG_INFO("OK!");

    /* Set the key and cert */
    LOG_INFO(fmt::format("Loading {}!", certPath));
    if (SSL_CTX_use_certificate_file(ctx, certPath, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    LOG_INFO(fmt::format("Loading {}!", keyPath));
    if (SSL_CTX_use_PrivateKey_file(ctx, keyPath, SSL_FILETYPE_PEM) <= 0)
    {
        LOG_INFO("Some sort of problem");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    LOG_INFO("Leaving function");
}

int main(int argc, char **argv)
{
    std::vector<Client *> clients;
    int                   sock;
    SSL_CTX              *ctx;

    if (argc < 3)
    {
        print_usage();
        return 1;
    }

    ctx = create_context();

    configure_context(ctx, argv[1], argv[2]);

    sock = create_socket(5000);

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