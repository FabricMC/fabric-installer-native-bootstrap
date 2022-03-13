#pragma once

#include <Windows.h>

#include <optional>
#include <string>
#include <vector>

class ISystemHelper {
public:
	virtual std::optional<std::wstring> getRegValue(HKEY hive, const std::wstring& path, const std::wstring& key) = 0;
	virtual std::optional<std::wstring> getEnvVar(const std::wstring& key) = 0;
	virtual void showErrorMessage(const std::wstring& title, const std::wstring& message, const std::wstring& url) = 0;
	virtual DWORD createProcess(const std::wstring& path, std::vector<std::wstring> args) = 0;
	virtual bool fileExists(const std::wstring& path) = 0;
	virtual bool dirExists(const std::wstring& path) = 0;
	virtual std::wstring getBootstrapFilename() = 0;
};