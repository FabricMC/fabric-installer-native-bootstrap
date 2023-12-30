#include "Logger.h"
#include <sstream>

namespace {
    std::wofstream openLogFile(ISystemHelper& systemHelper) {
		std::wstringstream fileNameBuf;
		fileNameBuf << systemHelper.getTempDir() << L"/fabric-installer-" << systemHelper.getEpochTime() << L".log";
        return std::wofstream(fileNameBuf.str());
	}
}

Logger::Logger(ISystemHelper& systemHelper) : file_(openLogFile(systemHelper))
{
}

Logger::~Logger()
{
	file_.close();
}