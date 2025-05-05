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
        return false;  // chemin invalide ou inaccessible

    return S_ISDIR(s.st_mode);  // vrai si c’est un dossier
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

static int	nbrlen(int n)
{
	int	i;

	i = 0;
	if (n <= 0)
		++i;
	while (n)
	{
		n /= 10;
		++i;
	}
	return (i);
}

void	*ft_memset(void *b, int c, size_t len)
{
	size_t			i;

	i = 0;
	while (i < len)
	{
		((unsigned char *) b)[i] = (unsigned char) c;
		++i;
	}
	return (b);
}

void	*ft_calloc(size_t count, size_t size)
{
	void	*mem;

	if (count != 0 && (count * size) / count != size)
		return (NULL);
	mem = malloc(size * count);
	if (!mem)
		return (NULL);
	ft_memset(mem, 0, size * count);
	return (mem);
}

char	*ft_itoa(int n)
{
	int		len;
	char	*number;

	if (n == INT_MIN)
		return (strdup("-2147483648"));
	len = nbrlen(n);
	number = (char *) ft_calloc(len + 1, sizeof(char));
	if (!number)
		return (NULL);
	if (n < 0)
	{
		number[0] = '-';
		n = -n;
	}
	while (len && number[--len] != '-')
	{
		number[len] = (n % 10) + '0';
		n /= 10;
	}
	return (number);
}
