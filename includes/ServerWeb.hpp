#pragma once
#include <sys/socket.h>
#include <iostream>
#include <errno.h>
#include "Socket.hpp"
#include "Config.hpp"

class ServerWeb
{
    public:
        ServerWeb(int ac , char **av);
    private:
        Config config;
        Socket socket;
};