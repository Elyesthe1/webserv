#include "../../../includes/Socket.hpp"

Socket::Socket(const Config &config)
{
    try
    {
        this->InitSocket(config);
    }
    catch(const std::exception& e)
    {
        Logger::ErrorLog("Socket", std::string(e.what()) + strerror(errno));
        exit(EXIT_FAILURE);
    }
}

Client Socket::AcceptClient()
{
    struct sockaddr_in addrr;
    socklen_t addrlen = sizeof(socklen_t);
    int newFd = accept(this->socketFD, (struct sockaddr*)&addrr, &addrlen);
    if (newFd == -1)
    {
        Logger::WarningLog("Socket", "accept() failed while trying to accept a new client connection: " + std::string(strerror(errno)));
        throw std::exception();
    }
    this->SetNonBlocking(newFd);
    return Client(newFd, addrr);
}

void Socket::InitSocket(const Config &config) 
{
    this->CreateSocket(config);
    this->SetNonBlocking(this->socketFD);
    this->SetSocketOp(config); // pas encore sure des option a mettre 
    this->BindSocket(config);
    this->Listen(config);
}

void Socket::SetNonBlocking(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1)
        throw std::runtime_error("Failed to set the socket in non blocking: ");
    if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) == -1)
        throw std::runtime_error("Failed to set the socket in non blocking: ");
}

void Socket::Listen(const Config &config) const
{
    if(listen(this->socketFD, 10) == -1)
        throw std::runtime_error("Failed to Listen the socket: ");
}

void Socket::BindSocket(const Config &config) const
{
    if (bind(this->socketFD, (struct sockaddr*)&config.Getaddr(), sizeof(config.Getaddr())) == - 1)
        throw std::runtime_error("Failed to bind the socket: ");
}
void Socket::CreateSocket(const Config &config)
{
    if((this->socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error("Failed to create socket: ");
}
// pas encore sure sure de cette fonction a voir, celon le fichier conf et tout
void Socket::SetSocketOp(const Config &config) const
{
    int option = 1;
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
