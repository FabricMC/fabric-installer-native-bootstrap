#pragma once

#include "ISystemHelper.h"
#include <fstream>

class Logger
{
public:
	Logger(ISystemHelper& systemHelper);
	~Logger();

	template<class T>
	void log(const T& line) {
		file_ << line << std::endl;
	}

private:
	std::wofstream file_;
};
