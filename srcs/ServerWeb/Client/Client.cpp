#include "../../../includes/Client.hpp"

Client::Client(const int fd, struct sockaddr_in addr) : ClientFD(fd), address(addr) {}

int Client::GetClientFd() const {return this->ClientFD;}

Client::~Client()
{
    this->CloseClientFd();
}

void Client::CloseClientFd()
{
    if (this->ClientFD != -1)
        close(this->ClientFD);
}
