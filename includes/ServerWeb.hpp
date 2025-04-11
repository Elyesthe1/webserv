#pragma once
#include <sys/socket.h>
#include <iostream>
#include <errno.h>
#include "Socket.hpp"
#include "Config.hpp"
#include "Client.hpp"
#include <map>
#include "Logger.hpp"
#include <sys/epoll.h>
#include <string>
const std::string intTostring(const int n);

class ServerWeb
{
    public:
        ServerWeb(int ac , char **av);
        void run();
        ~ServerWeb();
    private:
        void Send(const int &client);
	    struct epoll_event events[1024];
        void ClientHandler( const struct epoll_event &events);
        void DisconnectClient(const struct epoll_event &events);
        void NewClient();
		void FdLoop(int readyFD);
        int Epoll_Wait();
        void MainLoop();
        void launch();
        void CloseEpoll();
        void EpollInit();
        int epoll;
        void ReceiveData(const struct epoll_event &events);
        // std::map<int, Client> client;
        const Config config;
        Socket socket;
};
