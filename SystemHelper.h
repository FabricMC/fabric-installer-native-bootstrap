#pragma once

#include "ISystemHelper.h"

class SystemHelper : public ISystemHelper {
public:
	std::optional<std::wstring> getRegValue(HKEY hive, const std::wstring& path, const std::wstring& key) override;
	std::optional<std::wstring> getEnvVar(const std::wstring& key) override;
	void showErrorMessage(const std::wstring& title, const std::wstring& message, const std::wstring& url) override;
	DWORD createProcess(std::vector<std::wstring> args) override;
	bool fileExists(const std::wstring& path) override;
	bool dirExists(const std::wstring& path) override;
	std::wstring getBootstrapFilename() override;
};