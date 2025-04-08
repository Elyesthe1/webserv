#pragma once
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
class Client
{
    public:
        // int GetClientFd() const;
        Client(const int fd, struct sockaddr_in addr);
        ~Client();
    private:
        const std::string ip;
        void CloseClientFd();
        const int ClientFD;
        const struct sockaddr_in address;
};
