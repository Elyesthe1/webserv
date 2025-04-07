#pragma once
#include <iostream>
#include <fstream>
#define COLOR_RESET   "\033[0m"
#define GREEN    "\033[32m" 
#define YELLOW "\033[33m"
#define RED   "\033[31m"
#include <ctime>
#include <iomanip>
#include <sstream>
class Logger
{
	public:
		static void InfoLog(const std::string &service, const std::string &msg);
		static void WarningLog(const std::string &service, const std::string &msg);
		static void ErrorLog(const std::string &service, const std::string &msg);
};
