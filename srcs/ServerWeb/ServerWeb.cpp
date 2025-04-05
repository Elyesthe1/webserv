#include "../../includes/ServerWeb.hpp"

ServerWeb::ServerWeb(int ac, char **av): config(ac, av), socket(this->config)
{
}
