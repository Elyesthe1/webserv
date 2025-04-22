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

ServManager::~ServManager()
{
	this->ManageSignals(false);	
    this->CloseEpoll();
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
	if (fd == -1)
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


void ServManager::run()
{
    this->MainLoop();
}

void ServManager::InitLoop()
{
    for(int i = 0; i < this->conf.size(); i++)
    {
        try
        {
            ServerWeb srv(this->conf.GetConfig(i));
            this->AddToEpoll(srv.conf.GetFd());
            this->Map_Fd_Server[srv.conf.GetFd()] = srv;
            this->Map_Host_Server[srv.conf.GetHost()] = srv;
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
		ServerWeb::running = 0;
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

void ServManager::ClientHandler(const struct epoll_event &events)
{
	try
	{
		if (events.data.fd == this->socket.GetFd())
			this->NewClient();
		else
			this->ReceiveData(events);
	}
	catch (const std::exception &e)
	{
		Logger::WarningLog("ClientHandler", "Problem to handle client due to: " + std::string(e.what()));
	}

}