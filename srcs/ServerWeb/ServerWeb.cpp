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
		throw std::runtime_error("Cannot create epoll: ");
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = this->socket.GetFd();
	if (epoll_ctl(this->epoll, EPOLL_CTL_ADD, this->socket.GetFd(), &ev) == -1)
    	throw std::runtime_error("Cannot add listening socket to epoll: ");
}

int ServerWeb::Epoll_Wait(struct epoll_event events[1024])
{
	int fd = epoll_wait(this->epoll, events, 1024, -1);
	if (fd == -1)
    	throw std::runtime_error("epoll_wait failed: " + std::string(strerror(errno)));
	return fd;
}
void ServerWeb::FdLoop(struct epoll_event events[1024], int readyFD)
{
	std::cout << readyFD << std::endl;
}


void ServerWeb::MainLoop()
{
	while(1)
	{
		struct epoll_event events[1024];

		int readyFD = this->Epoll_Wait(events);
		this->socket.AcceptClient();
		FdLoop(events, readyFD);
		break;
	}
}

void ServerWeb::launch()
{
	this->EpollInit();
	Logger::InfoLog("Server", "Server started successfully! Listening on port " + intTostring(this->config.GetPorts()));
	this->MainLoop();
}

void ServerWeb::run() 
{
	try
	{
        this->launch();
	}
	catch(const std::exception& e)
	{
		Logger::ErrorLog("Server", "A critical error occurred. The server will shut down. Reason: " + std::string(e.what()));
	}
		
}
