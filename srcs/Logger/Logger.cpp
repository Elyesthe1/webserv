#include "../../includes/Logger.hpp"

const std::string GetCurrentTime()
{
    std::time_t now = std::time(NULL);
    std::tm *time = std::localtime(&now);

    std::ostringstream oss;
    oss << "[" 
        << std::setfill('0') << std::setw(4) << (1900 + time->tm_year) << "-"
        << std::setw(2) << (time->tm_mon + 1) << "-"
        << std::setw(2) << time->tm_mday << " "
        << std::setw(2) << time->tm_hour << ":"
        << std::setw(2) << time->tm_min << ":"
        << std::setw(2) << time->tm_sec 
        << "]";
    return oss.str() + " ";
}

void Logger::InfoLog(const std::string &service, const std::string &msg)
{
    std::cerr <<  GetCurrentTime() << GREEN << "[" << service + "] " << msg << COLOR_RESET << std::endl;  
}
void Logger::WarningLog(const std::string &service, const std::string &msg)
{
    std::cerr <<  GetCurrentTime() << YELLOW << "[" << service + "] " << msg << COLOR_RESET << std::endl;
}

void Logger::ErrorLog(const std::string &service, const std::string &msg)
{
    std::cerr <<  GetCurrentTime() << RED << "[" << service + "] " << "Error: " <<  msg << COLOR_RESET << std::endl;
}