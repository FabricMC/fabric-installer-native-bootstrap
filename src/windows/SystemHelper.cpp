#include "SystemHelper.h"

#include <Shlwapi.h>
#include <sstream>

std::optional<std::wstring> SystemHelper::getRegValue(HKEY hive, const std::wstring& path, const std::wstring& key) {
	DWORD dataSize{};
	LONG retCode = ::RegGetValueW(
		hive,
		path.c_str(),
		key.c_str(),
		RRF_RT_REG_SZ,
		nullptr,
		nullptr,
		&dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		return std::nullopt;
	}

	std::wstring data;
	data.resize(dataSize / sizeof(wchar_t));

	retCode = ::RegGetValueW(
		hive,
		path.c_str(),
		key.c_str(),
		RRF_RT_REG_SZ,
		nullptr,
		&data[0],
		&dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		return std::nullopt;
	}

	DWORD stringLengthInWchars = dataSize / sizeof(wchar_t);
	stringLengthInWchars--; // Exclude the NUL written by the Win32 API
	data.resize(stringLengthInWchars);

	return data;
}

std::optional<std::wstring> SystemHelper::getEnvVar(const std::wstring& key)
{
	// Read the size of the env var
	DWORD size = ::GetEnvironmentVariableW(key.c_str(), nullptr, 0);
	if (!size || ::GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
		return std::nullopt;
	}

	// Read the env var
	std::wstring value(size, L'\0');
	size = ::GetEnvironmentVariableW(key.c_str(), &value[0], size);
	if (!size || size >= value.size()) {
		return std::nullopt;
	}

	value.resize(size);
	return value;
}

void SystemHelper::showErrorMessage(const std::wstring& title, const std::wstring& message, const std::wstring& url) {
	int result = MessageBoxW(
		nullptr,
		message.c_str(),
		title.c_str(),
		MB_ICONWARNING | MB_YESNO
	);

	if (result == IDYES) {
		ShellExecuteW(nullptr, nullptr, url.c_str(), nullptr, nullptr, SW_SHOW);
	}
}

DWORD SystemHelper::createProcess(std::vector<std::wstring> args)
{
	STARTUPINFOW info;
	PROCESS_INFORMATION processInfo;

	ZeroMemory(&info, sizeof(info));
	ZeroMemory(&processInfo, sizeof(processInfo));

	std::wstringstream cls;
	for (auto& arg : args) {
		cls << L"\"" << arg << L"\" ";
	}
	std::wstring commandLine = cls.str();

	// Create the child process
	if (::CreateProcessW(
		nullptr,
		commandLine.data(),
		nullptr,
		nullptr,
		false,
		CREATE_DEFAULT_ERROR_MODE,
		nullptr,
		nullptr,
		&info,
		&processInfo)) {

		// Wait for exit
		::WaitForSingleObject(processInfo.hProcess, INFINITE);

		// Read exit code
		DWORD exitCode;
		::GetExitCodeProcess(processInfo.hProcess, &exitCode);

		// Clean up
		::CloseHandle(processInfo.hThread);
		::CloseHandle(processInfo.hProcess);

		return exitCode;
	}
	else {
		std::string msg = "Failed to create process: " + std::to_string(GetLastError());
		return -1;
		//            throw std::exception(msg.c_str());
	}
}

bool SystemHelper::fileExists(const std::wstring& path) {
	const auto attributes = ::GetFileAttributesW(path.c_str());
	return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool SystemHelper::dirExists(const std::wstring& path) {
	const auto attributes = ::GetFileAttributesW(path.c_str());
	return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}

std::wstring SystemHelper::getBootstrapFilename() {
	wchar_t moduleFileName[MAX_PATH] = { 0 };
	::GetModuleFileNameW(nullptr, moduleFileName, MAX_PATH);
	return moduleFileName;
}