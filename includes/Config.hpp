#pragma once
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
class Config
{
    public:
        Config(int ac, char **av);
        const struct sockaddr_in& Getaddr() const ;
    private:
        struct sockaddr_in address;

};