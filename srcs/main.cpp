#include "../includes/Config.hpp"
#include "../includes/ServerWeb.hpp"


int main(int ac, char **av)
{
    ServerWeb serv(ac, av);
    serv.run();
    return 0;
}
