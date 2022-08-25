#include <iostream>

#include "run.hpp"

static void printUsage(void)
{
    std::cout << "Usage: " << std::endl;
    std::cout << "./Client <ip> <port>" << std::endl;
}

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        printUsage();
        return 1;
    }
    run(argv[1], argv[2]);

    return 0;
}