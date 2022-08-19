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
#include "initServer.hpp"
#include "logging.hpp"
#include "processClients.hpp"
#include "safe_queue.hpp"
#include "ssl.hpp"

#define TRUE 1
#define FALSE 0

static void print_usage(void)
{
    std::cout << "Usage: " << std::endl;
    std::cout << "./Server <path-to-cert.pem> <path-to-key.pem>" << std::endl;
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