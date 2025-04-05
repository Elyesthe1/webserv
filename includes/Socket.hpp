#pragma once
#include <iostream>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "Config.hpp"
#include <exception>
#include <unistd.h>
class Socket
{
    public:
        Socket(Config &config);
        ~Socket();
    private:
        void CloseSocket();
        void InitSocket(Config &config);
        void Listen(Config &config) const ;
        void BindSocket(Config &config) const ;
        void SetSocketOp(Config &config) const;
        void CreateSocket(Config &config);
        int socketFD;
};