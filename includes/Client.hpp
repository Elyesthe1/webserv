#pragma once
#include <netinet/in.h>
#include <unistd.h>

class Client
{
    public:
        // int GetClientFd() const;
        Client(const int fd, struct sockaddr_in addr);
        ~Client();
    private:
        void CloseClientFd();
        const int ClientFD;
        const struct sockaddr_in address;
};