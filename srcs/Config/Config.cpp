#include "../../includes/Config.hpp"
#include <netinet/ip.h>
const struct sockaddr_in& Config::Getaddr() const {return this->address;}

int Config::GetPorts() const {return this->ports;}

std::string Config::GetRoot() const {return this->root;}

const std::string &Config::Get404() const {return this->Error404Path;}

std::string Config::GetIndex() const {return this->index;}

const std::string Config::GetUploadPath() const {return this->UploadPath;}

int Config::GetMaxBody() const {return this->MaxBody;}

bool Config::IsBodyLimited() const {return this->is_body_limited;}
Config Config::GetConfig(int i) const {return this->Vec_Conf[i];}
int Config::size() const {return this->How_Much_Server;}

Config::Config() {}
std::string Config::GetHost() const {return this->host;}

void Config::SetSocketAddrr(const int port)
{
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(8080);
    this->ports = 8080;
}

void Config::SetRoot(const std::string Root)
{
    this->root = Root;
}

void Config::SetIndex(const std::string Index)
{
    this->index = Index;
}

void   Config::Set404(const std::string path)
{
    this->Error404Path = path;
}
void Config::SetUpload(const std::string path)
{
    this->UploadPath = path;
}
void Config::SetBodyLimit(const int limit, bool islimited)
{
    this->MaxBody = limit;
    this->is_body_limited = islimited;
}

void Config::SetHost(const std::string Host)
{
    this->host = Host;
}



Config::Config(int ac, char **av)
{
    // setup juste pour test
    std::vector<std::string> root;
    root.push_back("www/casino");
    root.push_back("www/default");
    std::vector<int> port;
    port.push_back(8080);
    port.push_back(8081);
    std::vector<std::string> index;
    index.push_back("index.html");
    index.push_back("index.html");
    std::vector<std::string> page404;
    page404.push_back("www/default/errors/404.html");
    page404.push_back("www/default/errors/404.ht");
    std::vector<std::string> Upload;
    Upload.push_back("www/casino/uploads");
    Upload.push_back("www/default/uploads");
    std::vector<std::pair<int,int> > Limit;
    Limit.push_back(std::make_pair(0, 0));
    Limit.push_back(std::make_pair(100, 1));
    std::vector<std::string> host;
    host.push_back("");
    host.push_back("www.example.com");

    for(int i = 0; i < 1; i++)
    {
        Config conf;
        conf.SetSocketAddrr(port[i]);
        conf.SetRoot(root[i]);
        conf.SetIndex(index[i]);
        conf.Set404(page404[i]);
        conf.SetUpload(Upload[i]);
        conf.SetBodyLimit(Limit[i].first, Limit[i].second);
        conf.SetHost(host[i]);
        this->How_Much_Server = i + 1;
        this->Vec_Conf.push_back(conf);
    }
}
