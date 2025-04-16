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
	file.close();
	return buffer.str();
}

std::string ServerWeb::BuildHttpHeader(const int StatusCode, const std::string& ContentType, const size_t ContentLen) 
{
    std::string statusText;
    switch (StatusCode) 
	{
        case 200: statusText = "OK"; break;
        case 404: statusText = "Not Found"; break;
		case 204: statusText = "No Content"; break;
		case 403: statusText = "Forbidden"; break;
		default:  statusText = "Unknown"; break;
    }
    std::string header = "HTTP/1.1 " + intTostring(StatusCode) + " " + statusText + "\r\n";
    header += "Content-Type: " + ContentType + "\r\n";
    header += "Content-Length: " +	intTostring(ContentLen) + "\r\n";
    header += "Connection: close\r\n";
    header += "\r\n";
    return header;
}

std::string ServerWeb::BuildBody(std::string &FilePath, int &StatusCode)
{
	std::ifstream file(FilePath.c_str());
	if (!file)
	{
		StatusCode = 404;
		FilePath = "404.html";
		return this->Send404Page();
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return buffer.str();
}

std::string ServerWeb::GetContentType(const std::string& path)
{
    std::size_t dot = path.find_last_of(".");
    if (dot == std::string::npos)
    	return "application/octet-stream"; // par default 

    std::string ext = path.substr(dot + 1);
    if (ext == "html" || ext == "htm")
    	return "text/html";
    else if (ext == "css")
        return "text/css";
    else if (ext == "js")
        return "application/javascript";
    else if (ext == "png")
        return "image/png";
    else if (ext == "jpg" || ext == "jpeg")
        return "image/jpeg";
    else if (ext == "gif")
        return "image/gif";
    else if (ext == "ico")
        return "image/x-icon";
    else if (ext == "txt")
        return "text/plain";
    else if (ext == "json")
        return "application/json";
    else if (ext == "pdf")
        return "application/pdf";
    else
        return "application/octet-stream"; // par default si je coco pas 
}

std::string ServerWeb::BuildHttpResponse(int statusCode, const std::string& contentType, const std::string& body)
{
	std::string header = this->BuildHttpHeader(statusCode, contentType, body.size());
	return header + body;
}
void ServerWeb::Send(int clientFd, int statusCode, const std::string& contentType, const std::string& body)
{
	std::string response = this->BuildHttpResponse(statusCode, contentType, body);
	if (send(clientFd, response.c_str(), response.size(), 0) == -1)
		throw std::runtime_error(std::string("send() failed: ") + strerror(errno));
}

void ServerWeb::NewClient()
{
	int NewClient = this->socket.AcceptClient();
	struct epoll_event eve;
	eve.events = EPOLLIN ;
 	eve.data.fd = NewClient;
	if (epoll_ctl(this->epoll, EPOLL_CTL_ADD, NewClient, &eve) == -1)
		Logger::ErrorLog("epoll_ctl", "Add client to epoll failed " + std::string(strerror(errno)));
}

void ServerWeb::DisconnectClient(const struct epoll_event &events)
{
	if (epoll_ctl(this->epoll, EPOLL_CTL_DEL, events.data.fd,this->events) == -1) // pas obligele kernel le fait a ta place
		Logger::WarningLog("epoll_ctl", "Remove client to epoll failed" + std::string(strerror(errno)));
	close(events.data.fd);
	this->Vec_Client.erase(events.data.fd);
	if (events.events & EPOLLERR)
		Logger::WarningLog("Client", "⚠️ Unexpected disconnection (fd: " + intTostring(events.data.fd) + ")");
	else
		Logger::InfoLog("Client", "Client disconnected gracefully (fd: " + intTostring(events.data.fd) + ")");
}

void ServerWeb::GetMethod(std::string path, const int Client)
{
	int statuscode = 200;
	std::string body;
	std::string CompletePath;
	if (!path[1])
		CompletePath = this->config.GetRoot() + "/" + this->config.GetIndex();
	else
		CompletePath = this->config.GetRoot() + path;
	body = this->BuildBody(CompletePath, statuscode);
	this->Send(Client, statuscode,this->GetContentType(CompletePath), body);
}

std::string ServerWeb::GetPath(std::string Line)
{ 
	std::size_t pos = Line.find(' ', 0);
	if (pos == std::string::npos)
		return "/";
	return Line.substr(0, pos);
}

void ServerWeb::DeleteMethod(std::string path, const int Client)
{
	struct stat buffer;
    if (stat(path.c_str(), &buffer) == -1)
		this->Send(Client, 404, "", "");
	else if (std::remove(path.c_str()) != 0)
		this->Send(Client, 403, "", "");
	else
		this->Send(Client, 204, "", "");
}

void ServerWeb::PostMethod(std::string path, std::string body, const int Client)
{
	int pos = body.find("filename=");
	std::string name = path + "/" +  body.substr(pos + 11, body.find("\"", pos + 11) -  (pos + 11 )) ;
	std::ofstream of(name.c_str());	
	int contentstart = body.find("\r\n", pos);
	// of.write()
}

void ServerWeb::RequestParsing(std::string Request, const int Client)
{
	// std::cout << Request << std::endl;
	if (!std::strncmp(Request.c_str(), "GET", 3))
		this->GetMethod(this->GetPath(&Request[4]), Client);
	if (!std::strncmp(Request.c_str(), "DELETE", 6))
		this->DeleteMethod(this->config.GetRoot() + this->GetPath(&Request[7]), Client);
	if (!std::strncmp(Request.c_str(), "POST", 4))
		this->PostMethod(this->config.GetUploadPath(), Request, Client);
}

// JE DOIT TOUT REFAIRE, IL FAUT UTILISER FD_SET, FD_CLR, FD_ISSET, FD_ZERO
// void ServerWeb::RecvLoop(const int Client)
// {
// 	ssize_t status;
// 	char buffer[READ_BUFFER];
// 	std::string Data;
// 	std::size_t contentsize;
// 	std::string body;
// 	while (1)
// 	{
// 		status = recv(Client, buffer, sizeof(buffer), 0); // return 2 caractere en plus qui indique une fin de ligne " Carriage Return Line Feed"
// 		if (status > 0)
// 			Data.append(buffer, status);
// 		contentsize = Data.find("Content-Length:", 0);
// 		if (contentsize != std::string::npos)
// 		{
// 			int length = std::atoi(&Data[contentsize + 15]);
// 			while (length)
// 			{
// 				status = recv(Client, buffer, sizeof(buffer), 0); // return 2 caractere en plus qui indique une fin de ligne " Carriage Return Line Feed"
// 				if (status > 0)
// 				{
// 					body.append(buffer, status);
// 					length -= status;
// 				}
// 			}
// 		}
// 		if (status <= 0)
// 			break; // if -1 attention j'ai pas le droit t'utiliser errno donc, peut etre faut modifier des truc ici, car si faut ya pas d'erreur et juste le yenyen il a pas encore envoyer
// 	}
// 	std::cout << body << std::endl;
// 	std::cout << Data << std::endl;
// 	this->RequestParsing(Data, Client);
// }

bool ServerWeb::IsRequestComplete(const std::string& request)
{
	std::size_t pos = request.find("\r\n\r\n");
	if (pos == std::string::npos)
		return false;
	std::size_t contentLengthPos = request.find("Content-Length:");
	if (contentLengthPos != std::string::npos)
	{
		int length = std::atoi(request.c_str() + contentLengthPos + 15);
		std::size_t bodyStart = request.find("\r\n\r\n") + 4;
		if (request.size() >= bodyStart + length)
			return true;
		else
			return false;
	}
	return true;
}


// faire vector 
int ServerWeb::RecvLoop(const int Client)
{
	ssize_t status;
	char buffer[READ_BUFFER];
	std::string Data;
	while ((status = recv(Client, buffer, sizeof(buffer), 0)) > 0)
		Data.append(buffer, status);
	this->Vec_Client[Client].data += Data;
	if (status == 0)
	{
		this->RequestParsing(this->Vec_Client[Client].data, Client);
		this->Vec_Client[Client].data.clear();
		return 0;
	}
	else if (status == -1 && this->IsRequestComplete(this->Vec_Client[Client].data))
	{
		this->RequestParsing(this->Vec_Client[Client].data, Client);
		this->Vec_Client[Client].data.clear();
		return 0;
	}
	return 1;
}

void ServerWeb::ReceiveData(const struct epoll_event &events)
{
	if (this->RecvLoop(events.data.fd) == 0)
		this->DisconnectClient(events);
}

void ServerWeb::ClientHandler(const struct epoll_event &events)
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

