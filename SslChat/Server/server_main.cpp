#include <iostream>

#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <openssl/err.h>
#include <openssl/ssl.h>

#include "logging.hpp"

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
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(s, 1) < 0)
    {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Listening...";
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
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

int my_cb(char *buf, int size, int rwflag, void *u)
{
    std::cout << "Setting password, buffer size is " << size << std::endl;
    strncpy(buf, (char *)u, size);
    buf[size - 1] = '\0';
    return strlen(buf);
}
void configure_context(SSL_CTX *ctx, const char *certPath, const char *keyPath)
{
    SSL_CTX_set_default_passwd_cb_userdata(ctx, (void *)"test");
    SSL_CTX_set_default_passwd_cb(ctx, my_cb);

    /* Set the key and cert */
    std::cout << "Loading " << certPath << std::endl;
    if (SSL_CTX_use_certificate_file(ctx, certPath, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    std::cout << "Loading " << keyPath << std::endl;
    if (SSL_CTX_use_PrivateKey_file(ctx, keyPath, SSL_FILETYPE_PEM) <= 0)
    {
        std::cout << "Some sort of problem..." << std::endl;
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    std::cout << "Leaving function" << std::endl;
}

int main(int argc, char **argv)
{
    int      sock;
    SSL_CTX *ctx;

    if (argc < 3)
    {
        print_usage();
        return 1;
    }

    ctx = create_context();

    configure_context(ctx, argv[1], argv[2]);

    sock = create_socket(4433);

    std::cout << "Entering loop..." << std::endl;
    /* Handle connections */
    while (1)
    {
        struct sockaddr_in addr;
        unsigned int       len = sizeof(addr);
        SSL               *ssl;
        const char         reply[] = "test\n";

        std::cout << "Waiting for client..." << std::endl;
        int client = accept(sock, (struct sockaddr *)&addr, &len);
        if (client < 0)
        {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) <= 0)
        {
            ERR_print_errors_fp(stderr);
        }
        else
        {
            SSL_write(ssl, reply, strlen(reply));
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client);
    }

    close(sock);
    SSL_CTX_free(ctx);
}