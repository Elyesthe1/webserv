#pragma once
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <vector>
#include "Route.hpp"
#include <cstring>
const std::string intTostring(const int n);

class Config
{
    public:
        const std::string &Get404() const;
        const std::string &Get413() const;
        const std::string &Get500() const;
        const std::string &Get403() const;
        const std::string &Get400() const;
        const std::string &Get405() const;
        const std::string &Get501() const;
        const struct sockaddr_in& Getaddr(const int i) const ;
        int GetPorts(const int i) const;
        int GetVec_RoutesSize()const ;
        int GetMaxBody() const;
        Config GetConfig(const int i) const;
        static int size();
        std::string GetHost() const;
        int address_size() const;

        Config(int ac, char **av);
        Config();
        const Route* FindRoute(const std::string& path) const;
        
    private:
        void SetSocketAddrr(const int port);
        void Set404(const std::string path);
        void SetBodyLimit(const int limit);
        void SetHost(const std::string host);
        void Set413(const std::string path);
        void ParseFile(std::ifstream &file);
        void OpenFile(int ac , char **av);
        void DefaultConf();
    
        std::string Error405Path;
        std::string Error501Path;
        std::string Error413Path;
        std::string Error404Path;
        std::string Error500Path;
        std::string Error403Path;
        std::string Error400Path;
        std::string host;
        static int How_Much_Server;
        std::vector<Config> Vec_Conf;
        long long  MaxBody;
        std::vector<int> Vec_ports;
        std::vector<struct sockaddr_in> Vec_address;
        std::vector<Route> Vec_Routes;
};
