#pragma once
#include <sys/socket.h>
#include <iostream>
#include <errno.h>

class ServerWeb
{
    public:
        ServerWeb();
    private:
        int socketFD;
};