// standard headers
#include <thread>

// installed headers
#include <openssl/ssl.h>

// project headers
#include "logging.hpp"

class Client
{
    int         socket;
    SSL        *ssl;
    std::string name;

   public:
    void       log(std::string msg);
    inline int getSocket()
    {
        return this->socket;
    }
    inline SSL *getSsl()
    {
        return this->ssl;
    }
    int sendMessage(const char *message, size_t len);
    Client() = delete;
    Client(SSL *ssl, int socket);
    ~Client();
    inline operator std::string() const
    {
        return fmt::format("client_{}", this->socket);
    }

   private:
};
