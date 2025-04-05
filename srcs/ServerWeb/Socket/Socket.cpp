#include "../../../includes/Socket.hpp"

Socket::Socket(Config &config)
{
    if (!this->create(config))
        exit(EXIT_FAILURE);
    if (!this->Setsocketop(config))
        exit(EXIT_FAILURE);
    if (!this->Bind(config))
        exit(EXIT_FAILURE);
    if (!this->Listen(config))
        exit(EXIT_FAILURE);
}

bool Socket::Listen(Config &config)
{
    if(listen(this->socketFD, ))
}

bool Socket::Bind(Config &config)
{
    if (bind(this->socketFD, (struct sockaddr*)&config.Getaddr(), sizeof(config.Getaddr())) == - 1)
    {
        std::cerr << "Error: Failed to bind the socket: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}
bool Socket::create(Config &config)
{
    if((this->socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr << "Error: Failed to create socket: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Socket::Setsocketop(Config &config)
{
    int option = 1;
    if (setsockopt(this->socketFD, IPPROTO_TCP, TCP_NODELAY, &option, sizeof(option)) == -1) 
    {
        std::cerr << "Error: Failed to set socket option: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}