#pragma once
#include <sys/socket.h>
#include <iostream>
#include <errno.h>
#include "Socket.hpp"
#include "Config.hpp"
#include <map>
#include "Logger.hpp"
#include <sys/epoll.h>
#include <string>
#include <signal.h>
#include <sys/stat.h>
#include <cstdio>
#include <map>
#include <unistd.h>
#define NOT_FOUND_404 "<html><body><h1>404 Not Found</h1><p>Page introuvable</p></body></html>"
#define READ_BUFFER 4096
#define BodyTooLarge "<html><head><meta charset=\"UTF-8\"><title>413 Payload Too Large</title></head><body><h1>413 - Payload Too Large</h1><p>Le corps de la requête dépasse la taille autorisée.</p></body></html>"
const std::string intTostring(const int n);
class ServerWeb
{
    public:
        ServerWeb(Config conf);
        void run();
        ~ServerWeb();
    private:
        std::map<int, std::string>Vec_Client;
	    struct epoll_event events[1024];
        int epoll;
        static int running;
        Config config;
        Socket *socket;


        int RecvLoop(const int Client);
        int Epoll_Wait();
        static void SignalHandler(int Sig);
        void ManageSignals(bool flag);
        std::string BuildHttpResponse(int statusCode, const std::string& contentType, const std::string& body);
        std::string BuildHttpHeader(const int StatusCode, const std::string& ContentType, const size_t ContentLen);
        std::string Send404Page();
        std::string BuildBody(std::string &FilePath, int &StatusCode);
        void Send(int clientFd, int statusCode, const std::string& contentType, const std::string& body);
        void ClientHandler( const struct epoll_event &events);
        void DisconnectClient(const struct epoll_event &events);
        void NewClient();
		void FdLoop(const int readyFD);
        void MainLoop();
        void launch();
        void CloseEpoll();
        void EpollInit();
        void ReceiveData(const struct epoll_event &events);
        void RequestParsing(std::string Line, const int Client);
        void GetMethod(std::string Line, const int Client, std::string &Data);
        void DeleteMethod(std::string Line, const int Client);
        std::string GetPath(std::string line);
        std::string GetContentType(const std::string& path);
        int  IsRequestComplete(const std::string& request);
        void PostMethod(std::string path, std::string body, const int Client);
        void CGIMethod(std::string path, const int Client);
        bool CookieHandler(std::string &Data);
        void DeleteDynamiqueAllocation();
};
