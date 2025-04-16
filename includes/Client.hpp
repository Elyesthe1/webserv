#pragma once
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
class Client
{
    public:
        // int GetClientFd() const;
        Client();
        // Client(const int fd, struct sockaddr_in addr);
        ~Client();
        std::string data;
    private:
};
