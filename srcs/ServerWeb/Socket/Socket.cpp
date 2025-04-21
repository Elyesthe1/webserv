#include "../../../includes/Socket.hpp"

int Socket::GetFd() const {return this->socketFD;}

Socket::Socket(const Config &config)
{
    this->InitSocket(config);
}

int Socket::AcceptClient()
{
    struct sockaddr_in addrr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int newFd = accept(this->socketFD, (struct sockaddr*)&addrr, &addrlen);
    if (newFd == -1)
    {

        Logger::WarningLog("Socket", "accept() failed while trying to accept a new client connection: " + std::string(strerror(errno)));
        throw std::exception();
    }
    this->SetNonBlocking(newFd);
    Logger::InfoLog("Socket", "Accepted connection : (fd: " + intTostring(newFd) + ")");
    return newFd;
}

void Socket::InitSocket(const Config &config) 
{
    this->CreateSocket();
    this->SetNonBlocking(this->socketFD);
    this->SetSocketOp(); // pas encore sure des option a mettre 
    this->BindSocket(config);
    this->Listen();
}

void Socket::SetNonBlocking(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1)
    {
        Logger::WarningLog("FCNTL", "Failed to set the socket in non blocking: " + std::string(strerror(errno)));
        return ;
    }
    if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) == -1)
    {
        Logger::WarningLog("FCNTL", "Failed to set the socket in non blocking: " + std::string(strerror(errno)));
        return ;
    }
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
