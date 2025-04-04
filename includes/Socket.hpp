#pragma once
#include <iostream>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

class Socket
{
    public:
        Socket();
    private:
        int socketFD;
};