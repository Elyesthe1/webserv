#include "../includes/Config.hpp"
#include "../includes/ServManager.hpp"
#include "../includes/Logger.hpp"

int main(int ac, char **av)
{
    try
    {
        Config conf(ac, av);
        ServManager manager(conf);
        manager.run();
    }
    catch (const std::exception &e)
    {
        Logger::ErrorLog("Main", "Une erreur critique est survenue : " + std::string(e.what()));
    }
    return 0;
}
