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
#include <signal.h>
#define NOT_FOUND_404 "<html><body><h1>404 Not Found</h1><p>Page introuvable</p></body></html>"
#define READ_BUFFER 4096
const std::string intTostring(const int n);

class ServerWeb
{
    public:
        ServerWeb(int ac , char **av);
        void run();
        ~ServerWeb();
    private:
	    struct epoll_event events[1024];
        int epoll;
        const Config config;
        Socket socket;
        static int running;
        int RecvLoop(const int Client);
        int Epoll_Wait();
        static void SignalHandler(int Sig);
        void ManageSignals(bool flag);
        std::string BuildHttpResponse(const std::string &FilePath, int &StatusCode);
        std::string BuildHttpHeader(const int StatusCode, const std::string& ContentType, const size_t ContentLen);
        std::string Send404Page();
        std::string BuildBody(const std::string &FilePath, int &StatusCode);
        void Send(const int &client, const std::string &FilePath);
        void ClientHandler( const struct epoll_event &events);
        void DisconnectClient(const struct epoll_event &events);
        void NewClient();
		void FdLoop(const int readyFD);
        void MainLoop();
        void launch();
        void CloseEpoll();
        void EpollInit();
        void ReceiveData(const struct epoll_event &events);

};
