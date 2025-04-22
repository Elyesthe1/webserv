#include "../../../includes/Socket.hpp"

int Socket::GetFd() const {return this->socketFD;}

Socket::Socket(const Config &config) {this->InitSocket(config);}

void Socket::InitSocket(const Config &config) 
{
    this->CreateSocket();
    SetNonBlocking(this->socketFD);
    this->SetSocketOp();
    this->BindSocket(config);
    this->Listen();
}

void Socket::Listen() const
{
    if(listen(this->socketFD, MAX_CLIENT) == -1)
        throw std::runtime_error("Failed to Listen the socket: ");
}

void Socket::BindSocket(const Config &config) const
{
    if (bind(this->socketFD, (struct sockaddr*)&config.Getaddr(), sizeof(config.Getaddr())) == - 1)
        throw std::runtime_error("Failed to bind the socket: ");
}
void Socket::CreateSocket()
{
    if((this->socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error("Failed to create socket: ");
}
void Socket::SetSocketOp() const
{
    const int option = 1;
    if (setsockopt(this->socketFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)) == -1) 
        throw std::runtime_error("Failed to set socket option: ");
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
