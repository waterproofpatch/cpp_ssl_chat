// standard headers
#include <thread>

// installed headers
#include <openssl/ssl.h>

// project headers
#include "logging.hpp"

class Client
{
    int          socket;
    SSL         *ssl;
    std::thread *t;
    std::string  name;

   public:
    void start();
    void stop();
    void run();
    Client() = delete;
    Client(SSL *ssl, int socket);
    ~Client();
    inline operator std::string() const
    {
        return fmt::format("client_{}", this->socket);
    }

   private:
};
