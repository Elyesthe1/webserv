#include "../../includes/Config.hpp"
#include <netinet/ip.h>
const struct sockaddr_in& Config::Getaddr(const int i) const {return this->address[i];}

int Config::GetPorts(const int i) const {return this->ports[i];}

std::string Config::GetRoot() const {return this->root;}

const std::string &Config::Get404() const {return this->Error404Path;}
const std::string &Config::Get413() const {return this->Error413Path;}

int Config::address_size() const {return this->address.size();}

std::string Config::GetIndex() const {return this->index;}

const std::string Config::GetUploadPath() const {return this->UploadPath;}

int Config::GetMaxBody() const {return this->MaxBody;}

Config Config::GetConfig(int i) const {return this->Vec_Conf[i];}
int Config::size() const {return this->How_Much_Server;}

Config::Config() {}
std::string Config::GetHost() const {return this->host;}

void Config::SetSocketAddrr(const int port)
{
    struct sockaddr_in adrr;
    adrr.sin_family = AF_INET;
    adrr.sin_addr.s_addr = INADDR_ANY;
    adrr.sin_port = htons(port);
    this->address.push_back(adrr);
    this->ports.push_back(port);
}

void Config::SetRoot(const std::string Root) {this->root = Root;}

void Config::SetIndex(const std::string Index) {this->index = Index;}

void   Config::Set404(const std::string path) {this->Error404Path = path;}
void Config::SetUpload(const std::string path) {this->UploadPath = path;}
void Config::SetBodyLimit(const int limit) {this->MaxBody = limit;}

void Config::Set413(const std::string path) {this->Error413Path = path;}


void Config::SetHost(const std::string Host) {this->host = Host;}


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
    std::vector<std::string> page413;
    page413.push_back("www/default/errors/413.html");
    page413.push_back("www/default/errors/413.ht");
    std::vector<std::string> page404;
    page404.push_back("www/default/errors/404.html");
    page404.push_back("www/default/errors/404.ht");
    std::vector<std::string> Upload;
    Upload.push_back("www/casino/uploads");
    Upload.push_back("www/default/uploads");
    std::vector<int> Limit;
    Limit.push_back(-1);
    Limit.push_back(100);
    std::vector<std::string> host;
    host.push_back("");
    host.push_back("www.example.com");

    for(int i = 0; i < 2; i++)
    {
        Config conf;
        conf.SetSocketAddrr(port[i]);
        conf.SetRoot(root[i]);
        conf.SetIndex(index[i]);
        conf.Set404(page404[i]);
        conf.SetUpload(Upload[i]);
        conf.SetBodyLimit(Limit[i]);
        conf.SetHost(host[i]);
        this->How_Much_Server = i + 1;
        this->Vec_Conf.push_back(conf);
    }
}
