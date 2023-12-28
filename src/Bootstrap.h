#pragma once

#include <memory>

#include "ISystemHelper.h"
#include "Logger.h"

class Bootstrap {
public:
	explicit Bootstrap(ISystemHelper& systemHelper, Logger& logger);

public:
	void launch();

private:
	bool launchMinecraftLauncher();
	bool launchSystemJava();

	bool attemptLaunch(const std::wstring& path, bool checkExists);

	void showErrorMessage();

	const std::vector<std::wstring> getMinecraftJavaPaths(const HostArchitecture::Value& hostArch);

private:
	ISystemHelper& systemHelper;
	Logger& logger;
};