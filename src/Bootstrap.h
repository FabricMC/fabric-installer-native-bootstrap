#pragma once

#include <memory>
#include <optional>
#include <filesystem>

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

	const std::vector<std::wstring> getMinecraftJavaPaths(const Architecture::Value& hostArch);

	std::filesystem::path getExtractedInstaller();

private:
	ISystemHelper& systemHelper;
	Logger& logger;
	std::optional<std::filesystem::path> extractedInstaller = std::nullopt;
};