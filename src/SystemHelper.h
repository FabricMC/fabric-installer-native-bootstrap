#pragma once

#include "ISystemHelper.h"

class SystemHelper : public ISystemHelper {
public:
	std::optional<std::wstring> getRegValue(HKEY hive, const std::wstring& path, const std::wstring& key) const override;
	std::optional<std::wstring> getEnvVar(const std::wstring& key) const override;
	DWORD createProcess(std::vector<std::wstring> args) const override;
	bool fileExists(const std::wstring& path) const override;
	bool dirExists(const std::wstring& path) const override;
	std::wstring getBootstrapFilename() const override;
	std::wstring getTempDir() const override;
	Architecture::Value getHostArchitecture() const override;
	int64_t getEpochTime() const override;
};