#pragma once
#include <sys/socket.h>
#include <iostream>
#include <errno.h>
#include "Socket.hpp"
#include "Config.hpp"
#include "Client.hpp"
#include <map>
#include <sys/epoll.h>
class ServerWeb
{
    public:
        ServerWeb(int ac , char **av);
        void run();
        ~ServerWeb();
    private:
        void launch();
        void CloseEpoll();
        void EpollInit();
        int epoll;
        std::map<int, Client> client;
        const Config config;
        const Socket socket;
};
