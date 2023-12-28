#pragma once

#include <Windows.h>

#include <optional>
#include <string>
#include <vector>

namespace HostArchitecture {
	// Ordered to ensure that we only check for JVMs that will run on the host arch.
	// On an x64 host only check for x64 and x86 as arm will never run.
	// On x86 there is no need to try any other arch as it wont run.
	enum Value {
		UNKNOWN,
		ARM64,
		X64,
		X86,
	};

	constexpr Value VALUES[] = { UNKNOWN, ARM64, X64, X86 };

	inline std::wstring AsString(const Value& arch) {
		switch (arch) {
		case X64:
			return L"x64";
		case ARM64:
			return L"arm64";
		case X86:
			return L"x86";
		case UNKNOWN:
		default:
			return L"unknown";
		}
	}
}

class ISystemHelper {
public:
	virtual std::optional<std::wstring> getRegValue(HKEY hive, const std::wstring& path, const std::wstring& key) = 0;
	virtual std::optional<std::wstring> getEnvVar(const std::wstring& key) = 0;
	virtual void showErrorMessage(const std::wstring& title, const std::wstring& message, const std::wstring& url) = 0;
	virtual DWORD createProcess(std::vector<std::wstring> args) = 0;
	virtual bool fileExists(const std::wstring& path) = 0;
	virtual bool dirExists(const std::wstring& path) = 0;
	virtual std::wstring getBootstrapFilename() = 0;
	virtual std::wstring getTempDir() = 0;
	virtual HostArchitecture::Value getHostArchitecture() = 0;
	virtual long long getEpochTime() = 0;
};