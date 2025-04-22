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
std::string Config::GetHost() const {retunr this->host;}

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
    this->MaxBody = 0;
    this->is_body_limited = 0;
}

void Config::SetHost(const std::strin Host)
{
    this->host = host
}



Config::Config(int ac, char **av)
{
    // setup juste pour test
    Config conf;
    conf.SetSocketAddrr(8080);
    conf.SetRoot("www/casino");
    conf.SetIndex("index.html");
    conf.Set404("www/default/errors/404.html");
    conf.SetUpload("www/casino/uploads");
    conf.SetBodyLimit(0, 0);
    conf.SetHost("www.example.com");
    this->How_Much_Server = 1;
    this->Vec_Conf.push_back(conf);
    if (ac == 1)
    {
        std::ifstream default_conf("conf/default.conf");
        if (!default_conf)
        {
            std::cerr << "Erreur : Aucun fichier de configuration trouvé." << std::endl;
        }
    }
}
