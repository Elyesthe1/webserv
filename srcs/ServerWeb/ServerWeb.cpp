#include "../../includes/ServerWeb.hpp"

int ServerWeb::running = 1;

ServerWeb::ServerWeb(int ac, char **av): config(ac, av), socket(this->config) {}

ServerWeb::~ServerWeb()
{
    this->CloseEpoll();
	this->ManageSignals(false);	
}

void ServerWeb::CloseEpoll()
{
	if(this->epoll != -1)
		close(this->epoll);
}


void ServerWeb::EpollInit()
{
	if ((this->epoll = epoll_create(MAX_CLIENT)) == -1)
		throw std::runtime_error("Cannot create epoll: ");
	struct epoll_event events;
	events.events = EPOLLIN ;
	events.data.fd = this->socket.GetFd();
	if (epoll_ctl(this->epoll, EPOLL_CTL_ADD, this->socket.GetFd(), &events) == -1)
    	throw std::runtime_error("Cannot add listening socket to epoll: ");
}

int ServerWeb::Epoll_Wait()
{
	int fd = epoll_wait(this->epoll, this->events, MAX_CLIENT, -1);
	if (fd == -1)
    	throw std::runtime_error("epoll_wait failed: " + std::string(strerror(errno)));
	return fd;
}

std::string ServerWeb::Send404Page() 
{
	std::ifstream file(this->config.Get404().c_str());
	if (!file)
		return NOT_FOUND_404;
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string ServerWeb::BuildHttpHeader(const int StatusCode, const std::string& ContentType, const size_t ContentLen) 
{
    std::string statusText;
    switch (StatusCode) 
	{
        case 200: statusText = "OK"; break;
        case 404: statusText = "Not Found"; break;
		default:  statusText = "Unknown"; break;
    }
    std::string header = "HTTP/1.1 " + intTostring(StatusCode) + " " + statusText + "\r\n";
    header += "Content-Type: " + ContentType + "\r\n";
    header += "Content-Length: " +	intTostring(ContentLen) + "\r\n";
    header += "Connection: close\r\n";
    header += "\r\n";
    return header;
}

std::string ServerWeb::BuildBody(const std::string &FilePath, int &StatusCode)
{
	std::ifstream file(FilePath.c_str());
	if (!file)
	{
		StatusCode = 404;
		return this->Send404Page();
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string ServerWeb::BuildHttpResponse(const std::string &FilePath, int &StatusCode)
{
	std::string body = this->BuildBody(FilePath, StatusCode);
	std::string header = this->BuildHttpHeader(StatusCode, "text/html", body.size());
	return header + body;
}

void ServerWeb::Send(const int &clientFd, const std::string &FilePath)
{
	int StatusCode = 200;
	std::string Response = this->BuildHttpResponse(FilePath, StatusCode);
	send(clientFd, Response.c_str(), Response.size(), 0); // check return value ?
}

void ServerWeb::NewClient()
{
	int NewClient = this->socket.AcceptClient();
	struct epoll_event eve;
	eve.events = EPOLLIN ; // EPOLLOUT lorsque je rejoute le flag, pour savoir si il est pret que j ecrive, ca avance car en tcp les socket sont tjr pret a ecrire
 	eve.data.fd = NewClient;
	epoll_ctl(this->epoll, EPOLL_CTL_ADD, NewClient, &eve); // CHECK RETURN VALUE ?
	this->Send(NewClient, this->config.GetRoot());
}

void ServerWeb::DisconnectClient(const struct epoll_event &events)
{
	epoll_ctl(this->epoll, EPOLL_CTL_DEL, events.data.fd,this->events); // pas obligele kernel le fait a ta place // CHECK RETURN VALUE ?
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
	std::cout  << "\033[31m" << "READ: " << "\033[0m" << read << std::endl;
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


void ServerWeb::FdLoop(const int ReadyFD)
{
	for(int i = 0; i < ReadyFD; i++)
			this->ClientHandler(this->events[i]);
}


void ServerWeb::MainLoop()
{
	while(ServerWeb::running)
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

void ServerWeb::SignalHandler(int Sig)
{
	if (Sig == SIGINT)
	{
		Logger::InfoLog("SIGINT", "Serveur interrompu par SIGINT (Ctrl+C). Fermeture propre...");
		ServerWeb::running = 0;
	}
}

void ServerWeb::ManageSignals(bool flag)
{
	if (flag)
		signal(SIGINT, ServerWeb::SignalHandler);
	else
		signal(SIGINT, SIG_DFL);
}


void ServerWeb::run() 
{
	this->ManageSignals(true);	
	try
	{
        this->launch();
	}
	catch(const std::exception& e)
	{
		if (ServerWeb::running)
			Logger::ErrorLog("Server", "A critical error occurred. The server will shut down. Reason: " + std::string(e.what()));
	}
}
