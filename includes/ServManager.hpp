#pragma once
#include "Config.hpp"
#include "ServerWeb.hpp"
const std::string intTostring(const int n);
class ServManager
{
    public:
        ServManager(const Config &conf);
        ~ServManager();
    private:
        std::map<std::string, serve&r> Map_Host_Server;
        void run();
        const Config conf;
        static int running;
        std::map<int, ServerWeb&> Map_Fd_Server;
        struct epoll_event events[1024];
        int epoll;

        void FdLoop(const int ReadyFD);
        void ClientHandler(const struct epoll_event &events);
        void CloseEpoll();
        void launch();
        void InitServers();
        void AddToEpoll(const int fd);
        static void SignalHandler(int Sig);
        void ManageSignals(bool flag);
        void InitLoop();


};