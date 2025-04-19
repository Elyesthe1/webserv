#include "../../includes/Config.hpp"
#include <netinet/ip.h>
const struct sockaddr_in& Config::Getaddr() const {return this->address;}

int Config::GetPorts() const {return this->ports;}

std::string Config::GetRoot() const {return this->root;}

const std::string &Config::Get404() const {return this->Error404Path;}

std::string Config::GetIndex() const {return this->index;}

const std::string Config::GetUploadPath() const {return this->UploadPath;}

int Config::GetMaxBody() const {return this->MaxBody;}

bool Config::IsBodyLimited() const {return this->BodyLimit;}




Config::Config(int ac, char **av)
{
    // setup juste pour test
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(8080);
    this->ports = 8080;
    this->root = "www/casino";
    this->index = "index.html";
    this->Error404Path = "www/default/errors/404.html";
    this->UploadPath = "www/casino/uploads";
    this->MaxBody = 0;
    this->BodyLimit = 0;
    if (ac == 1)
    {
        std::ifstream default_conf("conf/default.conf");
        if (!default_conf)
        {
            std::cerr << "Erreur : Aucun fichier de configuration trouvé." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}
