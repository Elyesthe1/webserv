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
        std::string GetRoot() const;
        const std::string &Get404() const;
        std::string GetIndex() const ;
        const std::string GetUploadPath() const ;
        int GetMaxBody() const;
        bool IsBodyLimited ()const ;

    private:
        bool BodyLimit;
        int MaxBody;
        std::string UploadPath;
        std::string index;
        std::string Error404Path;
        std::string root;
        int ports;
        struct sockaddr_in address;
};
