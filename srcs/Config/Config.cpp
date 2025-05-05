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
    cgi.path = "/cgi-bin";
    cgi.root = "www/casino/cgi";
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

void Config::ParseFile(std::ifstream &file)
{
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
