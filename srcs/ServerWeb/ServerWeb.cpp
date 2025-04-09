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
	struct epoll_event events;
	events.events = EPOLLIN;
	events.data.fd = this->socket.GetFd();
	if (epoll_ctl(this->epoll, EPOLL_CTL_ADD, this->socket.GetFd(), &events) == -1)
    	throw std::runtime_error("Cannot add listening socket to epoll: ");
}

int ServerWeb::Epoll_Wait()
{
	int fd = epoll_wait(this->epoll, this->events, 1024, -1);
	if (fd == -1)
    	throw std::runtime_error("epoll_wait failed: " + std::string(strerror(errno)));
	return fd;
}

//            send(sockfd, buf, size, flags);

void ServerWeb::NewClient()
{
	int NewClient = this->socket.AcceptClient();
	struct epoll_event eve;
	eve.events = EPOLLIN;
	eve.data.fd = NewClient;
	epoll_ctl(this->epoll, EPOLL_CTL_ADD, NewClient, &eve);
	send(NewClient, "fdp bouffon\n", 12, 0);
}

void ServerWeb::DisconnectClient(const struct epoll_event &events)
{
	epoll_ctl(this->epoll, EPOLL_CTL_DEL, events.data.fd,this->events); // pas obligele kernel le fait a ta place 
	close(events.data.fd);
	if (events.events & EPOLLERR)
		Logger::WarningLog("Client", "⚠️ Unexpected disconnection (fd: " + intTostring(events.data.fd) + ")");
	else
		Logger::InfoLog("Client", "Client disconnected gracefully (fd: " + intTostring(events.data.fd) + ")");
}

void ServerWeb::ReceiveData(const struct epoll_event &events)
{
	char read[1024];
	int status = recv(events.data.fd, read, 1024, -1);
	if (status == -1)
		this->DisconnectClient(events);
}

void ServerWeb::ClientHandler( const struct epoll_event &events)
{
	if (events.data.fd == this->socket.GetFd())
			this->NewClient();
	else
		this->ReceiveData(events);
}


void ServerWeb::FdLoop(int ReadyFD)
{
	int i = 0;
	for(i = 0; i < ReadyFD; i++)
			this->ClientHandler(this->events[i]);
 
}


void ServerWeb::MainLoop()
{
	while(1)
	{
		int readyFD = this->Epoll_Wait();
		FdLoop(readyFD);
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
