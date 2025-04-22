#pragma once
#include <iostream>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "Config.hpp"
#include <exception>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include "Logger.hpp"
#define MAX_CLIENT 1024
const std::string intTostring(const int n);
void SetNonBlocking(const int fd);

class Socket
{
    public:
        Socket(const Config &config);
        ~Socket();
        int AcceptClient();
        int GetFd() const ;
    private:
        void CloseSocket();
        void InitSocket(const Config &config);
        void Listen() const ;
        void BindSocket(const Config &config) const ;
        void SetSocketOp() const;
        void CreateSocket();
        int socketFD;
};
