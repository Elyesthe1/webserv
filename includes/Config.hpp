#pragma once
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
class Config
{
    public:
        Config(int ac, char **av);
        const struct sockaddr_in& Getaddr() const ;
        int GetPorts() const;
        const std::string & GetRoot() const;
        const std::string &Get404() const;
        const std::string &GetIndex() const ;
    private:
        std::string index;
        std::string Error404Path;
        std::string root;
        int ports;
        struct sockaddr_in address;
};
