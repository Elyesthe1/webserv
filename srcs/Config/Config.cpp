#include "../../includes/Config.hpp"

Config::Config(int ac, char **av)
{
    if (ac == 1)
    {
        std::ifstream default_conf("conf/default.conf");
        if (!default_conf)
        {
            std::cerr << "conf error" << std::endl;
            exit(1);
        }
    }
}