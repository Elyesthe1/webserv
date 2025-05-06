#include "../../includes/Config.hpp"

int Config::How_Much_Server = 0;

const struct sockaddr_in& Config::Getaddr(const int i) const {return this->Vec_address[i];}

int Config::GetPorts(const int i) const {return this->Vec_ports[i];}

// std::string Config::GetRoot() const {return this->root;}


const std::string &Config::Get404() const {return this->Error404Path;}
const std::string &Config::Get413() const {return this->Error413Path;}
const std::string &Config::Get500() const {return this->Error500Path;}
const std::string &Config::Get403() const {return this->Error403Path;}
const std::string &Config::Get400() const {return this->Error400Path;} 
const std::string &Config::Get405() const {return this->Error405Path;}
const std::string &Config::Get501() const {return this->Error501Path;}
int Config::address_size() const {return this->Vec_address.size();}
int Config::GetVec_RoutesSize()const {return this->Vec_Routes.size();}


// std::string Config::GetIndex() const {return this->index;}

// const std::string Config::GetUploadPath() const {return this->UploadPath;}

int Config::GetMaxBody() const {return this->MaxBody;}

Config Config::GetConfig(int i) const {return this->Vec_Conf[i];}
int Config::size() {return Config::How_Much_Server;}

Config::Config() : MaxBody(-1){}

std::string Config::GetHost() const {return this->host;}

void Config::SetSocketAddrr(const int port)
{
	struct sockaddr_in adrr;
	adrr.sin_family = AF_INET;
	adrr.sin_addr.s_addr = INADDR_ANY;
	adrr.sin_port = htons(port);
	this->Vec_address.push_back(adrr);
	this->Vec_ports.push_back(port);
}

// void Config::SetRoot(const std::string Root) {this->root = Root;}

void   Config::Set404(const std::string path) {this->Error404Path = path;}
void Config::SetBodyLimit(const int limit) {this->MaxBody = limit;}

void Config::Set413(const std::string path) {this->Error413Path = path;}


void Config::SetHost(const std::string Host) {this->host = Host;}

const Route* Config::FindRoute(const std::string& path) const
{
	const Route* route = NULL;
	std::size_t len = 0;

	for (std::size_t i = 0; i < this->Vec_Routes.size(); ++i)
	{
		if (!std::strncmp(this->Vec_Routes[i].path.c_str(), path.c_str(), this->Vec_Routes[i].path.size()))
		{
			if (this->Vec_Routes[i].path != "/" && path.size() > this->Vec_Routes[i].path.size() && path[this->Vec_Routes[i].path.size()] != '/')
				continue;
			if (this->Vec_Routes[i].path.size() > len)
			{
				len = this->Vec_Routes[i].path.size();
				route = &this->Vec_Routes[i];
			}
		}
	}
	return route;
}


void Config::DefaultConf()
{
	this->How_Much_Server = 1;

	Config conf;
	conf.SetSocketAddrr(8080);

	conf.Error404Path = "www/casino/errors/404.html";
	conf.Error413Path = "www/default/errors/413.html";
	conf.Error403Path = "www/casino/errors/403.html";
	conf.Error400Path = "www/casino/errors/400.html";
	conf.Error500Path = "www/casino/errors/500.html";
	conf.Error405Path = "www/casino/errors/405.html";
	conf.Error501Path = "www/casino/errors/501.html";

	Route root;
	root.path = "/";
	root.root = "www/casino";
	root.index = "index.html";
	root.autoindex = false;
	root.methods.push_back("GET");
	root.methods.push_back("POST");
	root.methods.push_back("DELETE");

	Route uploads;
	uploads.path = "/upload";
	uploads.root = "www/casino/uploads";
	uploads.upload = "www/casino/uploads";
	uploads.methods.push_back("POST");

	

	Route redirect;
	redirect.path = "/redirect";
	redirect.redirection = "https://www.google.com";

	Route cgi;
	cgi.path = "/cgi-bin/";
	cgi.root = "/var/www/cgi-bin";
	cgi.methods.push_back("GET");
	cgi.methods.push_back("POST");

	Route images;
	images.path = "/images";
	images.root = "www/casino/uploads";
	images.index = "logan.jpg";

	conf.Vec_Routes.push_back(root);
	conf.Vec_Routes.push_back(uploads);
	conf.Vec_Routes.push_back(redirect);
	conf.Vec_Routes.push_back(cgi);
	conf.Vec_Routes.push_back(images);

	this->Vec_Conf.push_back(conf);
}

void Config::OpenFile(int ac, char **av)
{
	if (ac > 2)
		throw std::runtime_error("❌ Invalid number of arguments: expected './webserv <config_file>'");
	if (ac == 2)
	{ 
		std::ifstream file(av[1]);
		if(file)
			this->ParseFile(file);
		else
			throw std::runtime_error("❌ Failed to open configuration file: '" + std::string(av[1]) + "'");
	}
	else
		this->DefaultConf();
}

void trim(std::string &s) {
	size_t beg = s.find_first_not_of(" \t\r\n");
	size_t end = s.find_last_not_of(" \t\r\n");
	if (beg == std::string::npos)
		s = "";
	else
		s = s.substr(beg, end - beg + 1);
}

std::vector<std::string> splitWS(const std::string &s) {
	std::stringstream ss(s);
	std::string tok; std::vector<std::string> out;
	while (ss >> tok) out.push_back(tok);
	return out;
}

void Config::ParseFile(std::ifstream &file)
{
	std::string line;
	Config  current;
	Route   curLoc;
	bool    inLoc = false;

	while (std::getline(file, line))
	{
		size_t hash = line.find('#');
		if (hash != std::string::npos)
			line.erase(hash);
		trim(line);
		if (line.empty())
			continue ;

		if (!line.compare(0, 7, "server:"))
		{
			if (!current.Vec_ports.empty()) {
				if (inLoc)
					current.Vec_Routes.push_back(curLoc);
				this->Vec_Conf.push_back(current);
				++How_Much_Server;
			}
			current = Config();
			inLoc = false;
			continue ;
		}

		if (!line.compare(0, 9, "location_"))
		{
			if (inLoc)
				current.Vec_Routes.push_back(curLoc);
			curLoc = Route();
			inLoc  = true;

			size_t colon = line.find(':');
			std::string raw = line.substr(9, colon - 9);
			if (raw == "/")
				curLoc.path = "/";
			else {
				if (raw[0] != '/') raw.insert(raw.begin(), '/');
				curLoc.path = raw;
			}
			continue ;
		}

		size_t eq = line.find('=');
		if (eq == std::string::npos)
			throw std::runtime_error("⛔ config: ligne invalide -> " + line);

		std::string key = line.substr(0, eq);
		std::string val = line.substr(eq + 1);
		trim(key); trim(val);

		if (inLoc) {
			if (key == "root")                curLoc.root = val;
			else if (key == "index")          curLoc.index = val;
			else if (key == "autoindex")      curLoc.autoindex = (val == "on" || val == "true" || val == "1");
			else if (key == "redirection")    curLoc.redirection = val;
			else if (key == "upload")         curLoc.upload = val;
			else if (key == "cgi_extension")  curLoc.cgi_extension = val;
			else if (key == "allow_methods")  curLoc.methods = splitWS(val);
			else if (key == "client_max_body_size") current.SetBodyLimit(std::atoi(val.c_str()));
			else std::cerr << "⚠️  unknown location directive: " << key << std::endl;
		} else {
			if (key == "listen")              current.SetSocketAddrr(std::atoi(val.c_str()));
			else if (key == "server_name")    current.SetHost(val);
			else if (key == "client_max_body_size") current.SetBodyLimit(std::atoi(val.c_str()));
			else if (key == "error_page_404") current.Set404(val);
			else if (key == "error_page_413") current.Set413(val);
			else if (key == "error_page_403") current.Error403Path = val;
			else if (key == "error_page_400") current.Error400Path = val;
			else if (key == "error_page_500") current.Error500Path = val;
			else if (key == "error_page_405") current.Error405Path = val;
			else if (key == "error_page_501") current.Error501Path = val;
			else std::cerr << "⚠️  unknown server directive: " << key << std::endl;
		}
	}

	if (inLoc)
		current.Vec_Routes.push_back(curLoc);
	if (!current.Vec_ports.empty()) {
		this->Vec_Conf.push_back(current);
		++How_Much_Server;
	}
}

Config::Config(int ac, char **av)
{
	this->OpenFile(ac, av);
	// // setup juste pour test
	// std::vector<std::string> root;
	// root.push_back("www/casino");
	// root.push_back("www/default");
	// std::vector<int> port;
	// port.push_back(8080);
	// port.push_back(8080);
	// std::vector<std::string> index;
	// index.push_back("index.html");
	// index.push_back("index.html");
	// std::vector<std::string> page413;
	// page413.push_back("www/default/errors/413.html");
	// page413.push_back("www/default/errors/413.ht");
	// std::vector<std::string> page404;
	// page404.push_back("www/default/errors/404.html");
	// page404.push_back("www/default/errors/404.ht");
	// std::vector<std::string> Upload;
	// Upload.push_back("www/casino/uploads");
	// Upload.push_back("www/default/uploads");
	// std::vector<int> Limit;
	// Limit.push_back(-1);
	// Limit.push_back(100);
	// std::vector<std::string> host;
	// host.push_back("");
	// host.push_back("www.example.com");

	// for(int i = 0; i < 2; i++)
	// {
	//     Config conf;
	//     conf.SetSocketAddrr(port[i]);
	//     conf.SetRoot(root[i]);
	//     conf.SetIndex(index[i]);
	//     conf.Set404(page404[i]);
	//     conf.SetUpload(Upload[i]);
	//     conf.SetBodyLimit(Limit[i]);
	//     conf.SetHost(host[i]);
	//     this->How_Much_Server = i + 1;
	//     this->Vec_Conf.push_back(conf);
	// }
}
