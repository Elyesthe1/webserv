#pragma once
#include <iostream>
#include <fstream>
class Logger
{
	public:
		Logger(std::string name);
		void log(std::string msg);
	protected:
		const std::string name;
};
