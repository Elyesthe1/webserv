#include <sstream>

const std::string intTostring(const int n)
{
	std::ostringstream os;
	os << n;
	return os.str();
}
