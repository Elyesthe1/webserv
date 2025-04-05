#include "../../includes/Config.hpp"
#include <netinet/ip.h>
struct sockaddr_in Config::Getaddr() {return this->address;}

Config::Config(int ac, char **av)
{
    // setup juste pour test
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(80);
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