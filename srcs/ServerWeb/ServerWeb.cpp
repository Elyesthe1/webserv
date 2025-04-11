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
	events.events = EPOLLIN ;
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

void ServerWeb::Send(const int &client)
{
	std::ifstream file("index.html");
	std::stringstream content;
	content << file.rdbuf();
	std::string body = content.str();
	std::string response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + intTostring(body.size()) + "\r\n";
	response += "\r\n";
	response += body;
	send(client, response.c_str(), response.size(), 0);
}

void ServerWeb::NewClient()
{
	int NewClient = this->socket.AcceptClient();
	struct epoll_event eve;
	eve.events = EPOLLIN ; // EPOLLOUT lorsque je rejoute le flag, pour savoir si il est pret que j ecrive, ca avance car en tcp les socket sont tjr pret a ecrire
 	eve.data.fd = NewClient;
	epoll_ctl(this->epoll, EPOLL_CTL_ADD, NewClient, &eve);
	this->Send(NewClient);
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
	std::size_t status = recv(events.data.fd, read, sizeof(read), 0); // return 2 caractere en plus qui indique une fin de ligne " Carriage Return Line Feed"
	std::cout << read << std::endl;
	if (status == 0)
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
	for(int i = 0; i < ReadyFD; i++)
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
