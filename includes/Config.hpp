#pragma once
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <vector>
class Config
{
    public:
        Config(int ac, char **av);
        Config();
        const struct sockaddr_in& Getaddr() const ;
        int GetPorts() const;
        std::string GetRoot() const;
        const std::string &Get404() const;
        std::string GetIndex() const ;
        const std::string GetUploadPath() const ;
        int GetMaxBody() const;
        bool IsBodyLimited ()const ;
        Config GetConfig(const int i) const;
        int size() const ;
        void SetSocketAddrr(const int port);
        void SetRoot(const std::string root);
        void SetIndex(const std::string Index);
        void Set404(const std::string path);
        void SetUpload(const std::string path);
        void SetBodyLimit(const int limit, bool islimited);

    private:
        int How_Much_Server;
        std::vector<Config> Vec_Conf;
        bool is_body_limited;
        int MaxBody;
        std::string UploadPath;
        std::string index;
        std::string Error404Path;
        std::string root;
        int ports;
        struct sockaddr_in address;
};
