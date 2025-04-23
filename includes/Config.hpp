#pragma once
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <vector>
const std::string intTostring(const int n);

class Config
{
    public:
        Config(int ac, char **av);
        Config();
        const struct sockaddr_in& Getaddr(const int i) const ;
        int GetPorts(const int i) const;
        std::string GetRoot() const;
        const std::string &Get404() const;
        const std::string &Get413() const;
        std::string GetIndex() const ;
        const std::string GetUploadPath() const ;
        int GetMaxBody() const;
        Config GetConfig(const int i) const;
        int size() const ;
        std::string GetHost() const;
        int address_size() const;
        
    private:
        void SetSocketAddrr(const int port);
        void SetRoot(const std::string root);
        void SetIndex(const std::string Index);
        void Set404(const std::string path);
        void SetUpload(const std::string path);
        void SetBodyLimit(const int limit);
        void SetHost(const std::string host);
        void Set413(const std::string path);

        std::string Error413Path;
        std::string host;
        int How_Much_Server;
        std::vector<Config> Vec_Conf;
        long long  MaxBody;
        std::string UploadPath;
        std::string index;
        std::string Error404Path;
        std::string root;
        std::vector<int> ports;
        std::vector<struct sockaddr_in> address;
};
