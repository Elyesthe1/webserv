#include <sstream>
#include <fcntl.h>
#include <string.h>
#include "../../includes/Logger.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdlib.h> 
#include <sys/stat.h>

bool IsDirectory(const std::string& path)
{
    struct stat s;
    if (stat(path.c_str(), &s) != 0)
        return false;

    return S_ISDIR(s.st_mode);
}

const std::string intTostring(const int n)
{
	std::ostringstream os;
	os << n;
	return os.str();
}

int string_to_int(const std::string str)
{
	int number = 0;

	std::stringstream ss(str);
	ss >> number;

	return (number);
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

char to_lower(char c)
{
    return (static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
}
