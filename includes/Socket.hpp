#pragma once
#include <iostream>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "Config.hpp"
class Socket
{
    public:
        Socket(Config &config);
    private:
        bool Listen(Config &config);
        bool Bind(Config &config);
        bool Setsocketop(Config &config);
        bool create(Config &config);
        int socketFD;
};