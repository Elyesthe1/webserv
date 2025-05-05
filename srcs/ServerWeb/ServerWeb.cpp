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

std::string ServerWeb::get_cgi_file_path(const Route *route, std::string data)
{
	std::string file_name;
	std::string cgi_file_path;

	file_name =	get_cgi_path(data).substr(route->path.length());
	if (file_name.find('/') != std::string::npos)
		file_name = file_name.substr(1);

	cgi_file_path += "./";
	cgi_file_path += route->root;
	cgi_file_path += "/";

	cgi_file_path += file_name;
	cgi_file_path = cgi_file_path.substr(0, cgi_file_path.find("?"));

	return (cgi_file_path);
}

std::string ServerWeb::get_query_string(std::string data)
{
	const std::string path = this->GetPath(&data[4]);
	if (path.find('?') == std::string::npos)
		return ("");

	std::string query_string;
	int i = (data.rfind("GET", 0) == 0) ? 4 : 5;

	query_string = this->GetPath(&data[i]);
	query_string = query_string.substr(query_string.find("?") + 1);

	return (query_string);
}

char **ServerWeb::build_cgi_argv(std::string cgi_executable, std::string cgi_file_path)
{
	char **argv = new char*[3];

	argv[0] = new char[cgi_executable.length() + 1];
	argv[1] = new char[cgi_file_path.length() + 1];
	
	std::strcpy(argv[0], cgi_executable.c_str());
	std::strcpy(argv[1], cgi_file_path.c_str());
	argv[2] = NULL;

	return (argv);
}

std::string ServerWeb::get_server_protocol(std::string data)
{
	std::string server_protocol = data;

	if (server_protocol.find(' ') == std::string::npos)
		return ("");
	server_protocol = server_protocol.substr(server_protocol.find(' ') + 1);

	if (server_protocol.find(' ') == std::string::npos)
		return ("");
	server_protocol = server_protocol.substr(server_protocol.find(' ') + 1);

	if (server_protocol.find("\r\n") == std::string::npos)
		return ("");
	server_protocol = server_protocol.substr(0, server_protocol.find("\r\n"));

	return (server_protocol);
}

char **ServerWeb::build_cgi_get_envp(std::string cgi_file_path, std::string query_string, std::string data)
{
	char **envp = new char*[8];

	std::string server_protocol = get_server_protocol(data);
	std::string path_info = get_path_info(data);

	envp[0] = new char[std::strlen("CONTENT_TYPE=application/x-www-form-urlencoded") + 1];
	envp[1] = new char[std::strlen("QUERY_STRING=") + query_string.length() + 1];
	envp[2] = new char[std::strlen("REQUEST_METHOD=GET") + 1];
	envp[3] = new char[std::strlen("REDIRECT_STATUS=200") + 1];
	envp[4] = new char[std::strlen("SCRIPT_FILENAME=") + cgi_file_path.length() + 1];
	envp[5] = new char[std::strlen("SERVER_PROTOCOL=") + server_protocol.length() + 1];
	envp[6] = new char[std::strlen("PATH_INFO=") + path_info.length() + 1];

	std::strcpy(envp[0], "CONTENT_TYPE=application/x-www-form-urlencoded");
	std::strcpy(envp[1], "QUERY_STRING=");
	std::strcat(envp[1], query_string.c_str());
	std::strcpy(envp[2], "REQUEST_METHOD=GET");
	std::strcpy(envp[3], "REDIRECT_STATUS=200");
	std::strcpy(envp[4], "SCRIPT_FILENAME=");
	std::strcat(envp[4], cgi_file_path.c_str());
	std::strcpy(envp[5], "SERVER_PROTOCOL=");
	std::strcat(envp[5], server_protocol.c_str());
	std::strcpy(envp[6], "PATH_INFO=");
	std::strcat(envp[6], path_info.c_str());

	envp[7] = NULL;

	return (envp);
}


char **ServerWeb::build_cgi_post_envp(std::string cgi_file_path, std::string query_string, std::string data)
{
	std::string post_data = get_cgi_post_data(data);
	std::string server_protocol = get_server_protocol(data);
	std::string path_info = get_path_info(data);
	char **envp = new char*[9];
	char *itoa = ft_itoa(post_data.length());


	envp[0] = new char[std::strlen("CONTENT_TYPE=application/x-www-form-urlencoded") + 1];
	envp[1] = new char[std::strlen("QUERY_STRING=") + query_string.length() + 1];
	envp[2] = new char[std::strlen("REQUEST_METHOD=POST") + 1];
	envp[3] = new char[std::strlen("REDIRECT_STATUS=200") + 1];
	envp[4] = new char[std::strlen("SCRIPT_FILENAME=") + cgi_file_path.length() + 1];
	envp[5] = new char[std::strlen("CONTENT_LENGTH=") + std::strlen(itoa) + 1];
	envp[6] = new char[std::strlen("SERVER_PROTOCOL=") + server_protocol.length() + 1];
	envp[7] = new char[std::strlen("PATH_INFO=") + path_info.length() + 1];



	std::strcpy(envp[0], "CONTENT_TYPE=application/x-www-form-urlencoded");
	std::strcpy(envp[1], "QUERY_STRING=");
	std::strcat(envp[1], query_string.c_str());
	std::strcpy(envp[2], "REQUEST_METHOD=POST");
	std::strcpy(envp[3], "REDIRECT_STATUS=200");
	std::strcpy(envp[4], "SCRIPT_FILENAME=");
	std::strcat(envp[4], cgi_file_path.c_str());
	std::strcpy(envp[5], "CONTENT_LENGTH=");
	std::strcat(envp[5], itoa);
	std::strcpy(envp[6], "SERVER_PROTOCOL=");
	std::strcat(envp[6], server_protocol.c_str());
	std::strcpy(envp[7], "PATH_INFO=");
	std::strcat(envp[7], path_info.c_str());

	envp[8] = NULL;

	free(itoa);
	return (envp);
}

std::string	ServerWeb::get_cgi_content_type(std::string data)
{
	std::string lowercase = data;
	std::string	content_type = data;

	std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), to_lower);
	if (lowercase.find("content-type:") == std::string::npos)
		return ("text/html");
	
	content_type = content_type.substr(lowercase.find("content-type:"));
	content_type = content_type.substr(std::strlen("content-type:"));

	int i = 0;
	while (content_type[i] && content_type[i] == ' ')
		++i;
	content_type = content_type.substr(i);
	content_type = content_type.substr(0, content_type.find('\n'));
	
	return (content_type);
}

int ServerWeb::get_cgi_status_code(std::string data)
{
	std::string lowercase = data;
	std::string	status_code = data;

	std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), to_lower);
	if (lowercase.find("status:") == std::string::npos)
		return (200);
	
	status_code = status_code.substr(lowercase.find("status:"));
	status_code = status_code.substr(std::strlen("status:"));

	int i = 0;
	while (status_code[i] && status_code[i] == ' ')
		++i;
	status_code = status_code.substr(i);
	status_code = status_code.substr(0, status_code.find('\n'));
	
	return (string_to_int(status_code));
}

int ServerWeb::get_cgi_content_length(std::string data)
{
	std::string lowercase = data;
	std::string	content_length = data;

	std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), to_lower);
	if (lowercase.find("content-length:") == std::string::npos)
		return (-1);
	
	content_length = content_length.substr(lowercase.find("content-length:"));
	content_length = content_length.substr(std::strlen("content-length:"));

	int i = 0;
	while (content_length[i] && content_length[i] == ' ')
		++i;
	content_length = content_length.substr(i);
	content_length = content_length.substr(0, content_length.find('\n'));
	
	return (string_to_int(content_length));
}

std::string ServerWeb::get_cgi_body(std::string data)
{
	int content_length = -1;
	std::string body = data;
	std::string first_line = body.substr(0, data.find('\n'));

	std::transform(first_line.begin(), first_line.end(), first_line.begin(), to_lower);
	
	while (first_line.find("content-type:") != std::string::npos
		|| first_line.find("status:") != std::string::npos
		|| first_line.find("content-length:") != std::string::npos)
	{
		if (first_line.find("content-type:") != std::string::npos)
		{
			body = body.substr(first_line.length() + 1);
			first_line = body.substr(0, body.find('\n'));
			std::transform(first_line.begin(), first_line.end(), first_line.begin(), to_lower);
			continue;
		}
		else if (first_line.find("status:") != std::string::npos)
		{
			body = body.substr(first_line.length() + 1);
			first_line = body.substr(0, body.find('\n'));
			std::transform(first_line.begin(), first_line.end(), first_line.begin(), to_lower);
			continue;
		}
		else if (first_line.find("content-length:") != std::string::npos)
		{
			content_length = get_cgi_content_length(first_line);
			body = body.substr(first_line.length() + 1);
			first_line = body.substr(0, body.find('\n'));
			std::transform(first_line.begin(), first_line.end(), first_line.begin(), to_lower);
			continue;
		}
	}

	int i = 0;
	while (body[i] && body[i] == '\n')
		++i;
	body = body.substr(i);

	if (content_length != -1 && body.length() > static_cast<unsigned long>(content_length))
		body = body.substr(0, content_length);

	return (body);
}

std::string ServerWeb::get_path_info(std::string data)
{
	int i = (data.rfind("GET", 0) == 0) ? 4 : 5;
	std::string extension = (data.find(".py") != std::string::npos) ? ".py" : ".php";
	std::string path = this->GetPath(&data[i]);

	if (path.find('?') != std::string::npos)
		path.substr(0, path.find('?'));

	path = path.substr(path.find(extension) + extension.length());

	return (path);
}

std::string ServerWeb::get_cgi_path(std::string data)
{
	int i = (data.rfind("GET", 0) == 0) ? 4 : 5;
	std::string extension = (data.find(".py") != std::string::npos) ? ".py" : ".php";
	std::string path = GetPath(&data[i]);

	path = path.substr(0, path.find(extension) + extension.length());

	return (path);
}

void ServerWeb::CGI_GET(const int client, std::string data)
{
	const std::string cgi_executable = (data.find(".py") != std::string::npos) ? "/usr/bin/python3" : "/usr/bin/php-cgi";
	const Route *route = this->RouteCheck(get_cgi_path(data), client, "GET");

	if (!route)
		return Logger::InfoLog("CGI", "no route found");

	std::string cgi_file_path = this->get_cgi_file_path(route, data);
	std::string query_string = this->get_query_string(data);

	std::ifstream file(cgi_file_path.c_str());
	if (!file)
		return (this->Send(client, 404, "text/html", this->BuildErrorPage(404)));


	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		Logger::ErrorLog("CGI", "pipe creation failed");
		return (this->Send(client, 500, "text/html", this->BuildErrorPage(500)));
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		Logger::ErrorLog("CGI", "fork creation failed");
		return (this->Send(client, 500, "text/html", this->BuildErrorPage(500)));
	}
	else if (pid == 0)
	{
		char **argv = build_cgi_argv(cgi_executable, cgi_file_path);
		char **envp = build_cgi_get_envp(cgi_file_path, query_string, data);

		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		if (execve(cgi_executable.c_str(), argv, envp) == -1)
		{
			Logger::ErrorLog("CGI", "execve failed");
			delete[] argv[0];
			delete[] argv[1];
			delete[] argv;
			delete[] envp[0];
			delete[] envp[1];
			delete[] envp[2];
			delete[] envp[3];
			delete[] envp[4];
			delete[] envp[5];
			delete[] envp[6];
			delete[] envp;
			exit(1);
		}
	}
	else
	{
		int exit_status;

		close(pipefd[1]);

		std::string data;
		char buffer[10 + 1] = {0};

		while (read(pipefd[0], &buffer, 10) != 0)
		{
			data += buffer;
			std::fill_n(buffer, 10, 0);
		}
		close(pipefd[0]);
		waitpid(pid, &exit_status, 0);
		if (WEXITSTATUS(exit_status) == 1)
			return (this->Send(client, 500, "text/html", this->BuildErrorPage(500)));

		return (this->Send(client, get_cgi_status_code(data), get_cgi_content_type(data), get_cgi_body(data)));
	}
}

std::string ServerWeb::get_cgi_post_data(std::string data)
{
	std::string post_data = data.substr(data.find("\r\n\r\n") + std::strlen("\r\n\r\n"));

	return (post_data);
}

void ServerWeb::CGI_POST(const int client, std::string data)
{
	const std::string cgi_executable = (data.find(".py") != std::string::npos) ? "/usr/bin/python3" : "/usr/bin/php-cgi";
	const Route *route = RouteCheck(GetPath(&data[5]), client, "POST");

	if (!route)
		return Logger::InfoLog("CGI", "no route found");

	std::string cgi_file_path = get_cgi_file_path(route, data);
	std::string query_string = get_query_string(data);
	std::string post_data = get_cgi_post_data(data);


	std::ifstream file(cgi_file_path.c_str());
	if (!file)
 		return (this->Send(client, 404, "text/html", BuildErrorPage(404)));

	int in[2];
	int out[2];
	if (pipe(in) == -1)
	{
		Logger::ErrorLog("CGI", "in pipe creation failed");
		return this->Send(client, 500, "text/html", this->BuildErrorPage(500));
	}
	if (pipe(out) == -1)
	{
		close(in[0]);
		close(in[1]);
		Logger::ErrorLog("CGI", "pipe creation failed");
		return (this->Send(client, 500, "text/html", this->BuildErrorPage(500)));
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		close(in[0]);
		close(in[1]);
		close(out[0]);
		close(out[1]);
		Logger::ErrorLog("CGI", "fork creation failed");
		return (this->Send(client, 500, "text/html", this->BuildErrorPage(500)));
	}
	else if (pid == 0)
	{
		char **argv = build_cgi_argv(cgi_executable, cgi_file_path);
		char **envp = build_cgi_post_envp(cgi_file_path, query_string, data);

		dup2(in[0], STDIN_FILENO);
		close(in[0]);
		close(in[1]);

		dup2(out[1], STDOUT_FILENO);
		close(out[0]);
		close(out[1]);

		if (execve(cgi_executable.c_str(), argv, envp) == -1)
		{
			Logger::ErrorLog("CGI", "execve failed");
			delete[] argv[0];
			delete[] argv[1];
			delete[] argv;
			delete[] envp[0];
			delete[] envp[1];
			delete[] envp[2];
			delete[] envp[3];
			delete[] envp[4];
			delete[] envp[5];
			delete[] envp;
			exit(1);
		}
	}
	else
	{
		int exit_status;

		close(in[0]);
		close(out[1]);

		write(in[1], post_data.c_str(), post_data.length());
		close(in[1]);

		std::string data;
		char buffer[10 + 1] = {0};
		while (read(out[0], &buffer, 10) != 0)
		{
			data += buffer;
			std::fill_n(buffer, 10, 0);
		}
		close(out[0]);

		waitpid(pid, &exit_status, 0);
		if (WEXITSTATUS(exit_status) == 1)
			return (this->Send(client, 500, "text/html", this->BuildErrorPage(500)));

		return (this->Send(client, get_cgi_status_code(data), get_cgi_content_type(data), get_cgi_body(data)));
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
			this->CGI_GET(Client, Request);
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
			this->CGI_POST(Client, Request);
		else
			this->PostMethod(this->GetPath(&Request[5]), Request, Client);
	}
	else
		this->Send(Client, 501, "text/html", this->BuildErrorPage(501));
}
