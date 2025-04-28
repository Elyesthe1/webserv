#pragma once
#include "Config.hpp"
#include "ServerWeb.hpp"
#include <algorithm>
const std::string intTostring(const int n);
void SetNonBlocking(const int fd);

class ServerWeb;

class ServManager
{
    public:
        static int running;


        ServManager(const Config &conf);
        ~ServManager();
        void run();
    private:
        std::map<int, std::string> Map_Client_Data;
        std::map<std::string, ServerWeb*> Map_Host_Server;
        std::map<int, ServerWeb*> Map_Fd_Server;
        const Config conf;
        struct epoll_event events[1024];
        int epoll;

        void DestroyAllocation();
        std::string GetPath(std::string Line);
        int IsRequestComplete(const std::string& request);
        void RequestParsing(std::string Request, const int Client);
        ServerWeb *ExtractHost(const std::string& request);
        void DisconnectClient(const struct epoll_event &events);
        void ReceiveData(const struct epoll_event &events);
        int RecvLoop(const int Client);
        int AcceptClient(const int socket);
        void NewClient(const int socket);
        void EpollInit();
        void MainLoop();
        int Epoll_Wait();
        void FdLoop(const int ReadyFD);
        void ClientHandler(const struct epoll_event &events);
        void CloseEpoll();
        void launch();
        void InitServers();
        void AddToEpoll(const int fd);
        static void SignalHandler(const int Sig);
        void ManageSignals(const bool flag);
        void InitLoop();


};
