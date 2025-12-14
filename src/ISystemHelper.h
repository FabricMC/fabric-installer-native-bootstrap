#pragma once

#include <Windows.h>

#include <optional>
#include <string>
#include <vector>

#include "Architecture.h"

class ISystemHelper {
public:
	virtual std::optional<std::wstring> getRegValue(HKEY hive, const std::wstring& path, const std::wstring& key) const = 0;
	virtual std::optional<std::wstring> getEnvVar(const std::wstring& key) const = 0;
	virtual void showErrorMessage(const std::wstring& title, const std::wstring& message, const std::wstring& url) const = 0;
	virtual DWORD createProcess(const std::vector<std::wstring>& args) const = 0;
	virtual bool fileExists(const std::wstring& path) const = 0;
	virtual bool dirExists(const std::wstring& path) const = 0;
	virtual std::wstring getBootstrapFilename() const = 0;
	virtual std::wstring getTempDir() const = 0;
	virtual Architecture::Value getHostArchitecture() const = 0;
	virtual int64_t getEpochTime() const = 0;
};