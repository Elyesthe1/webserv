#include "../../includes/ServerWeb.hpp"

ServerWeb::ServerWeb(Config conf) : config(conf), socket(conf) {}

bool ServerWeb::CheckBodyLimit(std::string Data)
{
	if (this->config.GetMaxBody() == -1)
		return false;
	const std::size_t pos = Data.find("Content-Length:");
	if (pos == std::string::npos)
		return true;
	if (std::atoi(Data.c_str() + pos + 15) > this->config.GetMaxBody())
		return true;
	return false;
}

std::string ServerWeb::BuildErrorPage(const int status)
{
	std::ifstream file;
	switch(status)
	{
		case 404: file.open(this->config.Get404().c_str()); break;
		case 413: file.open(this->config.Get413().c_str()); break;
		case 403: file.open(this->config.Get403().c_str()); break;
		case 400: file.open(this->config.Get400().c_str()); break;
		case 500: file.open(this->config.Get500().c_str()); break;
		case 405: file.open(this->config.Get405().c_str()); break;
		default : file.open(this->config.Get501().c_str()); break;
	}
	if (!file)
	{
		switch(status)
		{
			case 404: return NOT_FOUND_404;
			case 413: return BodyTooLarge;
			case 403: return FORBIDDEN_403;
			case 400: return BAD_REQUEST_400;
			case 500: return INTERNALERROR;
			case 405: return METHOD_NOT_ALLOWED;
			default : return NOT_IMPLEMENTED;
		}
	}
	std::stringstream body;
	body << file.rdbuf();
	file.close();
	return body.str();
}

std::string ServerWeb::BuildHttpHeader(const int StatusCode, const std::string& ContentType, const size_t ContentLen) 
{
    std::string Status;
    switch (StatusCode) 
	{
        case 200: Status = "OK"; break;
        case 404: Status = "Not Found"; break;
		case 204: Status = "No Content"; break;
		case 413: Status = "Payload Too Large"; break;
		case 403: Status = "Forbidden"; break;
		case 400: Status = "Bad Request"; break;
		case 500: Status = "Internal Server Error"; break;
		case 501: Status = "Not Implemented"; break;
		case 405: Status = "Method Not Allowed"; break;
		default:  Status = "Unknown"; break;
    }
    std::string header = "HTTP/1.1 " + intTostring(StatusCode) + " " + Status + "\r\n";
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
		FilePath = ".html";
		return this->BuildErrorPage(404);
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return buffer.str();
}

std::string ServerWeb::GetContentType(const std::string& path)
{
    const std::size_t dot = path.find_last_of(".");
    if (dot == std::string::npos)
    	return "application/octet-stream"; 

    const std::string extention = path.substr(dot + 1);
    if (extention == "html" || extention == "htm")
    	return "text/html";
	else if (extention == "mp3" || extention == "m4a" )
		return "audio/mpeg";
    else if (extention == "css")
        return "text/css";
    else if (extention == "js")
        return "application/javascript";
    else if (extention == "png")
        return "image/png";
    else if (extention == "jpg" || extention == "jpeg")
        return "image/jpeg";
    else if (extention == "gif")
        return "image/gif";
    else if (extention == "ico")
        return "image/x-icon";
    else if (extention == "txt")
        return "text/plain";
    else if (extention == "json")
        return "application/json";
    else if (extention == "pdf")
        return "application/pdf";
    else
        return "application/octet-stream";
}

std::string ServerWeb::BuildHttpResponse(int statusCode, const std::string& contentType, const std::string& body)
{
	const std::string header = this->BuildHttpHeader(statusCode, contentType, body.size());
	return header + body;
}
void ServerWeb::Send(int clientFd, int statusCode, const std::string& contentType, const std::string& body)
{
	std::string response;
	if (statusCode)
		response = this->BuildHttpResponse(statusCode, contentType, body);
	else
		response = body;
	if (send(clientFd, response.c_str(), response.size(), 0) == -1)
		throw std::runtime_error(std::string("send() failed: ") + strerror(errno));
}


bool ServerWeb::CookieHandler(std::string &Data)
{
	int static visited = 0;
	const std::size_t PosCookie = Data.find("visitCount=");
	if (PosCookie != std::string::npos &&  std::atoi(Data.substr(PosCookie + 11).c_str()) == 1	)
		visited = 0;
	if (PosCookie != std::string::npos && std::atoi(Data.substr(PosCookie + 11).c_str()) == 9 && !visited)
	{
		visited++;
		return true;
	}
	return false;
}

std::string ServerWeb::AutoIndex(const std::string& Path, const std::string& Url)
{
    std::stringstream site;
    site << "<html><head><title>Index of " << Url << "</title></head><body>";
    site << "<h1>Index of " << Url << "</h1><ul>";

    DIR *dir = opendir(Path.c_str());
    if (!dir) 
		return "<h1>Failed to open directory</h1>";

    struct dirent *info;
    while ((info = readdir(dir)) != NULL)
        if (std::string(info->d_name) != ".") 
        	site << "<li><a href=\"" << Url << "/" << info->d_name << "\">" << info->d_name << "</a></li>";

    closedir(dir);
    site << "</ul></body></html>";
    return site.str();
}


std::string ServerWeb::BuildRedirectionHeader(const std::string redirection)
{
	std::ostringstream stream;
	stream << "HTTP/1.1 301 Moved Permanently\r\n";
	stream << "Location: " << redirection << "\r\n";
	stream << "Content-Length: 0\r\n\r\n";
	return stream.str();
}


void ServerWeb::GetMethod(std::string path, const int Client, std::string &Data)
{
	int statuscode = 200;
	std::string body;
	const Route *route = this->RouteCheck(path, Client, "GET");
	if (!route)
		return ;
	if (!route->redirection.empty())
		return this->Send(Client, 0, "", this->BuildRedirectionHeader(route->redirection));

	std::string relativePath = path.substr(route->path.length());
	if (!relativePath.empty() && relativePath[0] == '/')
		relativePath = relativePath.substr(1);
	
	std::string CompletePath = route->root + "/" + relativePath;
	if ( !path[1] && this->CookieHandler(Data))
		CompletePath = "www/casino/10th_visit.html";
	else if (IsDirectory(CompletePath))
	{
		if(!route->index.empty())
			CompletePath = route->root + "/" + route->index;
		else if (route->autoindex)
			return this->Send(Client, 200, "text/html", this->AutoIndex(CompletePath, path));
		else
			return this->Send(Client, 403, "text/html", this->BuildErrorPage(403));
	}
	body = this->BuildBody(CompletePath, statuscode);
	this->Send(Client, statuscode,this->GetContentType(CompletePath), body);
}

const Route *ServerWeb::RouteCheck(std::string path, int const Client, const std::string method)
{
	const Route *route = this->config.FindRoute(path);
	if (!route)
	{
		this->Send(Client, 404, "text/html", this->BuildErrorPage(404));
		return NULL;
	}
	if (!route->methods.empty())
	{
		if (std::find(route->methods.begin(), route->methods.end(), method) == route->methods.end())
		{
			this->Send(Client, 405, "text/html", this->BuildErrorPage(405));
			return NULL;
		}
	}
	return route;
}


void ServerWeb::DeleteMethod(std::string path, const int Client)
{
	const Route *route = this->RouteCheck(path, Client, "DELETE");
	if (!route)
		return ;
	std::string FullPath = route->root + path;
	struct stat buffer;
    if (stat(FullPath.c_str(), &buffer) == -1)
		this->Send(Client, 404, "text/html", this->BuildErrorPage(404));
	else if (IsDirectory(FullPath))
		return this->Send(Client, 403, "text/html", this->BuildErrorPage(403));
	else if (std::remove(FullPath.c_str()) != 0)
		this->Send(Client, 403, "text/html", this->BuildErrorPage(403));
	else
		this->Send(Client, 204, "", "");
}

void ServerWeb::PostMethod(std::string path, std::string Data, const int Client)
{
	const Route *route = this->RouteCheck(path, Client, "POST");
	if (!route)
		return ;
	
    const std::size_t BoundaryStart = Data.find("boundary=");
    if (BoundaryStart == std::string::npos)
        return this->Send(Client, 400, "text/html", this->BuildErrorPage(400));

    const std::size_t boundaryEnd = Data.find("\r\n", BoundaryStart);
    const std::string boundary = "--" + Data.substr(BoundaryStart + 9, boundaryEnd - (BoundaryStart + 9)); 

   	std::size_t File = Data.find("filename=");
    if (File == std::string::npos)
        return this->Send(Client, 400, "text/html", this->BuildErrorPage(400));

    const std::size_t startName = Data.find("\"", File + 9) + 1;
    const std::size_t endName = Data.find("\"", startName);
    const std::string filename = Data.substr(startName, endName - startName);
    const std::string fullpath = route->upload + "/" + filename;

    std::size_t contentStart = Data.find("\r\n\r\n", endName);
    if (contentStart == std::string::npos)
        return this->Send(Client, 400, "text/html", this->BuildErrorPage(400));
    contentStart += 4;


    const std::size_t contentEnd = Data.find(boundary, contentStart);
    if (contentEnd == std::string::npos)
		return this->Send(Client, 400, "text/html", this->BuildErrorPage(400));

    std::ofstream of(fullpath.c_str());
	if (!of)
		return this->Send(Client, 500, "text/html", this->BuildErrorPage(500));
    of.write(Data.c_str() + contentStart, contentEnd - contentStart - 2);
    of.close();
	this->Send(Client, 204, "", "");
}

// GET CGI
void ServerWeb::CGIMethod(std::string Data, const int Client)
{
	const std::string exec = Data.find(".py") != std::string::npos ? "/usr/bin/python3" : "/usr/bin/php-cgi";
	const Route *route = this->RouteCheck(this->GetPath(&Data[4]), Client, "GET");

	if (!route)
		return Logger::WarningLog("CGI", "no route found");

	std::string cgi_file_path = "." + route->root + this->GetPath(&Data[4]).substr(route->path.length());
	std::string form_data = cgi_file_path.substr(cgi_file_path.find("?") + 1);
	cgi_file_path = cgi_file_path.substr(0, cgi_file_path.find("?"));


	// std::cout << route->root << std::endl;
	// std::cout << route->path << std::endl;
	// std::cout << "path: " << this->GetPath(&Data[4]).substr(route->path.length()) << std::endl;
	// std::cout << "path: " << (route->root + this->GetPath(&Data[4]).substr(route->path.length())) << std::endl;

	std::cout << "cgi_file_path: " << cgi_file_path << std::endl;
	std::cout << "form_data: " << form_data << std::endl;

	std::ifstream file(cgi_file_path.c_str());
	if (!file)
	{
		std::cerr << "file not found" << std::endl;
		return this->Send(Client, 404, "text/html", this->BuildErrorPage(404));
	}
	int pipes[2];
	if (pipe(pipes) == -1)
	{
		Logger::ErrorLog("CGI", "pipes failed");
		return this->Send(Client, 500, "text/html", this->BuildErrorPage(500));
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		close(pipes[0]);
		close(pipes[1]);
		Logger::ErrorLog("CGI", "fork failed");
		return this->Send(Client, 500, "text/html", this->BuildErrorPage(500));
	} 
	else if (pid == 0)
	{
		char *argv[3];
		char *envp[4];

		// this->Send(Client, 200, "text/html", "<p>lol</p>");

		argv[0] = new char[exec.size() + 1];
		argv[1] = new char[cgi_file_path.size() + 1];
		std::strcpy(argv[0], exec.c_str());
		std::strcpy(argv[1], cgi_file_path.c_str());
		argv[2] = NULL;

		envp[0] = new char[std::strlen("CONTENT_TYPE=application/x-www-form-urlencoded") + 1];
		envp[1] = new char[std::strlen("QUERY_STRING=") + form_data.size() + 1];
		std::strcpy(envp[0], "CONTENT_TYPE=application/x-www-form-urlencoded");
		std::strcpy(envp[1], "QUERY_STRING=");
		std::strcat(envp[1], form_data.c_str());
		envp[2] = NULL;
	
		std::cout << "envp[0] = " << envp[0] << std::endl;
		std::cout << "envp[1] = " << envp[1] << std::endl;

		std::cout << "argv[0] = " << argv[0] << std::endl;
		
		close(pipes[0]);
		dup2(pipes[1], STDOUT_FILENO);
		close(pipes[1]);
		execve(exec.c_str(), argv, envp);
		exit(0);
		// ServManager::running = 0;
		// exit(0);
	}
	else 
	{

		char buffer[10 + 1] = {0};
		std::string data;
		close(pipes[1]);
		waitpid(pid, NULL, 0);

		while (read(pipes[0], &buffer, 10) != 0)
		{
			// std::cout << "buffer:" << buffer << std::endl;
			data.append(buffer);
			std::fill_n(buffer, 10, 0);
		}
		std::cout << data << std::endl;
		close(pipes[0]);
		this->Send(Client, 200, "text/html", data);
	}
}

std::string ServerWeb::GetPath(std::string Line)
{ 
	const std::size_t pos = Line.find(' ', 0);
	if (pos == std::string::npos)
		return "/";
	return Line.substr(0, pos);
}

void ServerWeb::RequestParsing(std::string Request, const int Client)
{
	std::cout << Request << std::endl;
	if (!std::strncmp(Request.c_str(), "GET", 3))
	{
		if(Request.find(".py") != std::string::npos || Request.find(".php") != std::string::npos)
			this->CGIMethod(Request, Client);
		else
			this->GetMethod(this->GetPath(&Request[4]), Client, Request);
	}
	else if (!std::strncmp(Request.c_str(), "DELETE", 6))
		this->DeleteMethod(this->GetPath(&Request[7]), Client);
	else if (!std::strncmp(Request.c_str(), "POST", 4))
	{
		if (this->CheckBodyLimit(Request))
			return this->Send(Client, 413, "text/html", this->BuildErrorPage(413));
		else if(Request.find(".py") != std::string::npos || Request.find(".php") != std::string::npos)
			this->CGIMethod(Request, Client);
		else
			this->PostMethod(this->GetPath(&Request[5]), Request, Client);
	}
	else
		this->Send(Client, 501, "text/html", this->BuildErrorPage(501));
}
