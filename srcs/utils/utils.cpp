#include <sstream>
#include <fcntl.h>
#include <string.h>
#include "../../includes/Logger.hpp"
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/stat.h>

bool IsDirectory(const std::string& path)
{
    struct stat s;
    if (stat(path.c_str(), &s) != 0)
        return false;  // chemin invalide ou inaccessible

    return S_ISDIR(s.st_mode);  // vrai si c’est un dossier
}

const std::string intTostring(const int n)
{
	std::ostringstream os;
	os << n;
	return os.str();
}

void SetNonBlocking(const int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1)
    {
        Logger::WarningLog("FCNTL", "Failed to set the socket in non blocking: " + std::string(strerror(errno)));
        return ;
    }
    if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) == -1)
    {
        Logger::WarningLog("FCNTL", "Failed to set the socket in non blocking: " + std::string(strerror(errno)));
        return ;
    }
}
