#include "../../includes/ServManager.hpp"

int ServManager::running = 1;


void ServManager::AddToEpoll(const int fd)
{
    struct epoll_event events;
	events.events = EPOLLIN ;
	events.data.fd = fd;
	if (epoll_ctl(this->epoll, EPOLL_CTL_ADD, fd, &events) == -1)
    	throw std::runtime_error("Cannot add listening socket to epoll: ");
}

void ServManager::DestroyAllocation()
{
	for(std::map<std::string, ServerWeb*>::iterator it = this->Map_Host_Server.begin(); it != this->Map_Host_Server.end();it++)
		delete it->second;
}


ServManager::~ServManager()
{
	this->ManageSignals(false);	
    this->CloseEpoll();
	this->DestroyAllocation();
}

void ServManager::CloseEpoll()
{
	if(this->epoll != -1)
		close(this->epoll);
}


ServManager::ServManager(const Config &conf) : conf(conf)
{
    this->ManageSignals(true);
    this->InitServers();
    this->run();
}

int ServManager::Epoll_Wait()
{
	int fd = epoll_wait(this->epoll, this->events, MAX_CLIENT, -1);
	if (fd == -1 && std::string(strerror(errno)) != "Interrupted system call" )
    		throw std::runtime_error("epoll_wait failed: " + std::string(strerror(errno)));
	return fd;
}

void ServManager::FdLoop(const int ReadyFD)
{
	for(int i = 0; i < ReadyFD; i++)
			this->ClientHandler(this->events[i]);
}

void ServManager::MainLoop()
{
	while(ServManager::running)
	{
		int readyFD = this->Epoll_Wait();
		FdLoop(readyFD);
	}
}
std::string ServManager::GetPath(std::string Line)
{ 
	std::size_t pos = Line.find(' ', 0);
	if (pos == std::string::npos)
		return "/";
	return Line.substr(0, pos);
}

void ServManager::run()
{
	Logger::InfoLog("ServerManager", "ServerManager started successfully!");
    this->MainLoop();
}

void ServManager::InitLoop()
{
    for(int i = 0; i < this->conf.size(); i++)
    {
        try
        {
            ServerWeb *srv = new ServerWeb(this->conf.GetConfig(i));
            this->AddToEpoll(srv->socket.GetFd());
            this->Map_Fd_Server[srv->socket.GetFd()] = srv;
            this->Map_Host_Server[srv->config.GetHost()] = srv;
        }
        catch (const std::exception &e)
        {
            if (i == 0)
                throw std::runtime_error("Critical: Failed to initialize the first server. Aborting startup.");
            Logger::WarningLog("[!] Skipping server " + intTostring(i), "Reason: " + std::string(e.what()));
        }
    }
}

void ServManager::InitServers()
{
    try
    {
        this->EpollInit();
        this->InitLoop();
    }
    catch (const std::exception &e)
    {
        Logger::ErrorLog("[✖] Fatal: Server initialization failed.", "Reason: " + std::string(e.what()));
    }
}


void ServManager::SignalHandler(int Sig)
{
	if (Sig == SIGINT)
	{
		Logger::InfoLog("SIGINT", "Serveur interrompu par SIGINT (Ctrl+C). Fermeture propre...");
		ServManager::running = 0;
	}
}

void ServManager::ManageSignals(bool flag)
{
	if (flag)
		signal(SIGINT, ServManager::SignalHandler);
	else
		signal(SIGINT, SIG_DFL);
}

void ServManager::EpollInit()
{
	if ((this->epoll = epoll_create(MAX_CLIENT)) == -1)
		throw std::runtime_error("Cannot create epoll: ");
}

int ServManager::AcceptClient(const int socket)
{
    struct sockaddr_in addrr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int newFd = accept(socket, (struct sockaddr*)&addrr, &addrlen);
    if (newFd == -1)
    {

        Logger::WarningLog("Socket", "accept() failed while trying to accept a new client connection: " + std::string(strerror(errno)));
        throw std::exception();
    }
    SetNonBlocking(newFd);
    Logger::InfoLog("Socket", "Accepted connection : (fd: " + intTostring(newFd) + ")");
    return newFd;
}

void ServManager::NewClient(const int socket)
{
	int NewClient = this->AcceptClient(socket);
	struct epoll_event eve;
	eve.events = EPOLLIN ;
 	eve.data.fd = NewClient;
	if (epoll_ctl(this->epoll, EPOLL_CTL_ADD, NewClient, &eve) == -1)
		Logger::ErrorLog("epoll_ctl", "Add client to epoll failed " + std::string(strerror(errno)));
}

void ServManager::DisconnectClient(const struct epoll_event &events)
{
	if (epoll_ctl(this->epoll, EPOLL_CTL_DEL, events.data.fd,this->events) == -1) // pas obligele kernel le fait a ta place
		Logger::WarningLog("epoll_ctl", "Remove client to epoll failed" + std::string(strerror(errno)));
	close(events.data.fd);
	Logger::InfoLog("Client", "Client disconnected gracefully (fd: " + intTostring(events.data.fd) + ")");
}

void ServManager::ReceiveData(const struct epoll_event &events)
{
	if (this->RecvLoop(events.data.fd) == 0)
		this->DisconnectClient(events);
}

int ServManager::IsRequestComplete(const std::string& request)
{
	std::size_t pos = request.find("\r\n\r\n");
	if (pos == std::string::npos)
		return 0;
	std::size_t contentLenPos = request.find("Content-Length:");
	if (contentLenPos != std::string::npos)
	{
		int length = std::atoi(request.c_str() + contentLenPos + 15);
		std::size_t bodyStart = request.find("\r\n\r\n") + 4;
		if (request.size() >= bodyStart + length)
			return 1;
		else
			return 0;
	}
	return 1;
}

ServerWeb *ServManager::ExtractHost(const std::string& request)
{
	std::size_t hostpos = request.find("Host:");
	if (hostpos == std::string::npos)
		return 	this->Map_Host_Server.begin()->second;

	std::size_t hostposend = request.find(":", hostpos+ 6);
	if (hostposend == std::string::npos)
		return 	this->Map_Host_Server.begin()->second;

	std::string host = request.substr(hostpos + 6 , hostposend - (hostpos + 6));
	if (this->Map_Host_Server.count(host))
		return this->Map_Host_Server[host];

	return this->Map_Host_Server.begin()->second;
}


void ServManager::RequestParsing(std::string Request, const int Client)
{
	ServerWeb *serv = this->ExtractHost(Request);
	if (!std::strncmp(Request.c_str(), "GET", 3))
	{
		if(Request.find(".py") != std::string::npos || Request.find(".php") != std::string::npos)
			serv->CGIMethod(Request, Client);
		else
			serv->GetMethod(this->GetPath(&Request[4]), Client, Request);
	}
	else if (!std::strncmp(Request.c_str(), "DELETE", 6))
		serv->DeleteMethod(serv->config.GetRoot() + this->GetPath(&Request[7]), Client);
	else if (!std::strncmp(Request.c_str(), "POST", 4))
	{
		if (serv->CheckBodyLimit(Request))
			serv->Send(Client, 413, "text/html", BodyTooLarge);
		else if(Request.find(".py") != std::string::npos || Request.find(".php") != std::string::npos)
			serv->CGIMethod(Request, Client);
		else
			serv->PostMethod(serv->config.GetUploadPath(), Request, Client);
	}
}


int ServManager::RecvLoop(const int Client)
{
	ssize_t status;
	char buffer[READ_BUFFER];
	while ((status = recv(Client, buffer, sizeof(buffer), 0)) > 0)
		this->Map_Client_Data[Client].append(buffer, status);	
	if (status == 0)
	{
		this->RequestParsing(this->Map_Client_Data[Client], Client);
		this->Map_Client_Data[Client].clear();
		return 0;
	}
	else if (status == -1 && this->IsRequestComplete(this->Map_Client_Data[Client]))
	{
		this->RequestParsing(this->Map_Client_Data[Client], Client);
		this->Map_Client_Data[Client].clear();
		return 0;
	}
	return 1;
}

void ServManager::ClientHandler(const struct epoll_event &events)
{
	try
	{
		if (this->Map_Fd_Server.count(events.data.fd))
			this->NewClient(events.data.fd);
		else
			this->ReceiveData(events);
	}
	catch (const std::exception &e)
	{
		Logger::WarningLog("ClientHandler", "Problem to handle client due to: " + std::string(e.what()));
	}

}
