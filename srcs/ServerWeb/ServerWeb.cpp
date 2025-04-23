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

std::string ServerWeb::Send413Page() 
{
	std::ifstream file(this->config.Get413().c_str());
	if (!file)
		return BodyTooLarge;
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return buffer.str();
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
	const std::string response = this->BuildHttpResponse(statusCode, contentType, body);
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


void ServerWeb::GetMethod(std::string path, const int Client, std::string &Data)
{
	int statuscode = 200;
	std::string body;
	std::string CompletePath;
	if (!path[1])
	{
		if (this->CookieHandler(Data))
			CompletePath = this->config.GetRoot() + "/" + "10th_visit.html";
		else
			CompletePath = this->config.GetRoot() + "/" + this->config.GetIndex();
	}
	else
		CompletePath = this->config.GetRoot() + path;
	body = this->BuildBody(CompletePath, statuscode);
	this->Send(Client, statuscode,this->GetContentType(CompletePath), body);
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

void ServerWeb::PostMethod(std::string path, std::string Data, const int Client)
{
    const std::size_t BoundaryStart = Data.find("boundary=");
    if (BoundaryStart == std::string::npos)
        return this->Send(Client, 400, "", "Missing boundary");

    const std::size_t boundaryEnd = Data.find("\r\n", BoundaryStart);
    const std::string boundary = "--" + Data.substr(BoundaryStart + 9, boundaryEnd - (BoundaryStart + 9)); 

   	std::size_t File = Data.find("filename=");
    if (File == std::string::npos)
        return this->Send(Client, 400, "", "Missing filename");

    const std::size_t startName = Data.find("\"", File + 9) + 1;
    const std::size_t endName = Data.find("\"", startName);
    const std::string filename = Data.substr(startName, endName - startName);
    const std::string fullpath = path + "/" + filename;

    std::size_t contentStart = Data.find("\r\n\r\n", endName);
    if (contentStart == std::string::npos)
        return this->Send(Client, 400, "", "Missing content start");
    contentStart += 4;


    const std::size_t contentEnd = Data.find(boundary, contentStart);
    if (contentEnd == std::string::npos)
		return this->Send(Client, 400, "", "Missing content end");

    std::ofstream of(fullpath.c_str(), std::ios::binary);
	if (!of)
		return this->Send(Client, 500, "", "Failed to open file");
    of.write(Data.c_str() + contentStart, contentEnd - contentStart - 2);
    of.close();
	this->Send(Client, 204, "", "");
}

void ServerWeb::CGIMethod(std::string Data, const int Client)
{
	const std::string exec = Data.find(".py") != std::string::npos ? "/usr/bin/python3" : "/usr/bin/php-cgi";
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
	if (!std::strncmp(Request.c_str(), "GET", 3))
	{
		if(Request.find(".py") != std::string::npos || Request.find(".php") != std::string::npos)
			this->CGIMethod(Request, Client);
		else
			this->GetMethod(this->GetPath(&Request[4]), Client, Request);
	}
	else if (!std::strncmp(Request.c_str(), "DELETE", 6))
		this->DeleteMethod(this->config.GetRoot() + this->GetPath(&Request[7]), Client);
	else if (!std::strncmp(Request.c_str(), "POST", 4))
	{
		if (this->CheckBodyLimit(Request))
			this->config.Get413();
		else if(Request.find(".py") != std::string::npos || Request.find(".php") != std::string::npos)
			this->CGIMethod(Request, Client);
		else
			this->PostMethod(this->config.GetUploadPath(), Request, Client);
	}
}
