#include "../../includes/ServerWeb.hpp"

ServerWeb::ServerWeb()
{
    if((this->socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr << "Error: Failed to create socket (server): " << strerror(errno) << std::endl;
        exit(errno);
    }
}
