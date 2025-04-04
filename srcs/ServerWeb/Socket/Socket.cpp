#include "../../../includes/Socket.hpp"

Socket::Socket()
{
    if((this->socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr << "Error: Failed to create socket (server): " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    int option = 1;
    if (setsockopt(this->socketFD, IPPROTO_TCP, TCP_NODELAY, &option, sizeof(option)) == -1) 
    {
        std::cerr << "Erreur set socket option" << std::endl;
        exit(EXIT_FAILURE);
    }
}