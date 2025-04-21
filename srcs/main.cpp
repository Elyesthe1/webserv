#include "../includes/Config.hpp"
#include "../includes/ServerWeb.hpp"

int main(int ac, char **av)
{
    Config conf(ac, av);
    for(int i = 0; i < conf.size(); i++)
    {
        ServerWeb serv(conf.GetConfig(i));
        serv.run();
    }
    return 0;
}
