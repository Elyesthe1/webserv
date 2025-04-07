#pragma once
#include <iostream>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "Config.hpp"
#include <exception>
#include <unistd.h>
#include "Client.hpp"
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include "Logger.hpp"
class Socket
{
    public:
        Socket(const Config &config);
        ~Socket();
        Client AcceptClient();
    private:
        void CloseSocket();
        void InitSocket(const Config &config);
        void Listen(const Config &config) const ;
        void BindSocket(const Config &config) const ;
        void SetSocketOp(const Config &config) const;
        void CreateSocket(const Config &config);
        void SetNonBlocking(int fd);
        int socketFD;
};
