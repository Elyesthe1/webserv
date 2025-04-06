#pragma once
#include <sys/socket.h>
#include <iostream>
#include <errno.h>
#include "Socket.hpp"
#include "Config.hpp"
#include "Client.hpp"
#include <map>
class ServerWeb
{
    public:
        ServerWeb(int ac , char **av);
        void run();
    private:
        std::map<int, Client> client;
        const Config config;
        const Socket socket;
};