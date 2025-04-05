#include "../../../includes/Socket.hpp"

Socket::Socket(Config &config)
{
    try
    {
        this->InitSocket(config);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }   
}

void Socket::InitSocket(Config &config) 
{
    this->CreateSocket(config);
    this->SetSocketOp(config);
    this->BindSocket(config);
    this->Listen(config);
}


void Socket::Listen(Config &config) const
{
    if(listen(this->socketFD, 10) == -1)
        throw std::runtime_error("Error: Failed to Listen the socket: ");
}

void Socket::BindSocket(Config &config) const
{
    if (bind(this->socketFD, (struct sockaddr*)&config.Getaddr(), sizeof(config.Getaddr())) == - 1)
        throw std::runtime_error("Error: Failed to bind the socket: ");
}
void Socket::CreateSocket(Config &config)
{
    if((this->socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error("Error: Failed to create socket: ");
}
// pas encore sure sure de cette fonction a voir, celon le fichier conf et tout
void Socket::SetSocketOp(Config &config) const
{
    int option = 1;
    if (setsockopt(this->socketFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)) == -1) 
        throw std::runtime_error("Error: Failed to set socket option: ");
}

void Socket::CloseSocket()
{
    if (this->socketFD != -1)
        close(this->socketFD);
}

Socket::~Socket()
{
    this->CloseSocket();
}