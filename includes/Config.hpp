#pragma once
#include <iostream>
#include <fstream>
#include <sys/socket.h>
class Config
{
    public:
        Config(int ac, char **av);
        struct sockaddr_in Getaddr();
    private:
        struct sockaddr_in address;

};