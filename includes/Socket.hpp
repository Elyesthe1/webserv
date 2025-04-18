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

class Socket
{
    public:
        Socket(const Config &config);
        ~Socket();
        int AcceptClient();
        int GetFd() const ;
    private:
        const std::string GetIp(const in_addr &addrr);
        void CloseSocket();
        void InitSocket(const Config &config);
        void Listen(const Config &config) const ;
        void BindSocket(const Config &config) const ;
        void SetSocketOp(const Config &config) const;
        void CreateSocket(const Config &config);
        void SetNonBlocking(int fd);
        int socketFD;
};
