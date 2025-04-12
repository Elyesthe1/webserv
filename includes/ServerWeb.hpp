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
#define NOT_FOUND_404 "<html><body><h1>404 Not Found</h1><p>Page introuvable</p></body></html>"
const std::string intTostring(const int n);

class ServerWeb
{
    public:
        ServerWeb(int ac , char **av);
        void run();
        ~ServerWeb();
    private:
        std::string BuildHttpHeader(const int StatusCode, const std::string& ContentType, const size_t ContentLen);
        std::string Send404Page();
        std::string BuildBody(const std::string &FilePath, int &StatusCode);
        void Send(const int &client, const std::string &FilePath);
	    struct epoll_event events[1024];
        void ClientHandler( const struct epoll_event &events);
        void DisconnectClient(const struct epoll_event &events);
        void NewClient();
		void FdLoop(const int readyFD);
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
