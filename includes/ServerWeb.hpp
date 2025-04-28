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
#include <unistd.h>
#include "Route.hpp"
#include <algorithm>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include "ServManager.hpp"
#define READ_BUFFER 4096
#define NOT_FOUND_404 "<html><body><h1>404 Not Found</h1><p>Page introuvable</p></body></html>"
#define BodyTooLarge "<html><head><meta charset=\"UTF-8\"><title>413 Payload Too Large</title></head><body><h1>413 - Payload Too Large</h1><p>Le corps de la requête dépasse la taille autorisée.</p></body></html>"
#define INTERNALERROR "<html><body><h1>500 - Internal Server Error</h1><p>Une erreur s'est produite sur le serveur.</p></body></html>"
#define FORBIDDEN_403 "<html><body><h1>403 - Forbidden</h1><p>Accès interdit à cette ressource.</p></body></html>"
#define BAD_REQUEST_400 "<html><body><h1>400 - Bad Request</h1><p>Votre requête est invalide.</p></body></html>"
#define METHOD_NOT_ALLOWED "<html><body><h1>🚫 Erreur 405</h1><p>La méthode HTTP utilisée n'est pas autorisée pour cette ressource.</p></body></html>"
#define NOT_IMPLEMENTED "<html><body><h1>🛠️ Erreur 501</h1><p>La méthode HTTP utilisée n'est pas reconnue ou non prise en charge par ce serveur.</p></body></html>"




const std::string intTostring(const int n);
bool IsDirectory(const std::string& path);
class ServerWeb
{
    public:
        ServerWeb(Config conf);
        void run();
        Config config;
        Socket socket;
        void RequestParsing(std::string Request, const int Client);
    private:
        const Route *RouteCheck(std::string path, const int Client, const std::string method);
        std::string AutoIndex(const std::string& Path, const std::string& Url);
        std::string BuildErrorPage(const int status);
        std::string GetPath(std::string Line);
        void PostMethod(std::string path, std::string body, const int Client);
        void CGIMethod(std::string path, const int Client);
        void GetMethod(std::string Line, const int Client, std::string &Data);
        void DeleteMethod(std::string Line, const int Client);
        bool CheckBodyLimit(std::string Data);
        void Send(int clientFd, int statusCode, const std::string& contentType, const std::string& body);
        std::string BuildHttpResponse(int statusCode, const std::string& contentType, const std::string& body);
        std::string BuildHttpHeader(const int StatusCode, const std::string& ContentType, const size_t ContentLen);
        std::string BuildRedirectionHeader(const std::string redirection);
        std::string BuildBody(std::string &FilePath, int &StatusCode);
        std::string GetContentType(const std::string& path);
        int  IsRequestComplete(const std::string& request);
        bool CookieHandler(std::string &Data);
};
