#include "../../includes/ServerWeb.hpp"

ServerWeb::ServerWeb(int ac, char **av): config(ac, av), socket(this->config) {}

ServerWeb::~ServerWeb()
{
    this->CloseEpoll();
}

void ServerWeb::CloseEpoll()
{
	if(this->epoll != -1)
		close(this->epoll);
}


void ServerWeb::EpollInit()
{
	if ((this->epoll = epoll_create(1024)) == -1)
		throw std::runtime_error("Error: Cannot create epoll: ");
}
void ServerWeb::launch()
{
	this->EpollInit();
}

void ServerWeb::run() 
{
	try
	{
        Logger::InfoLog("Server", "Server started successfully! Listening on port " + std::to_string(this->config.GetPorts()));
        this->launch();
	}
	catch(const std::exception& e)
	{
        Logger::ErrorLog("ServerWeb", std::string(e.what()) + strerror(errno));
	}
		
}
