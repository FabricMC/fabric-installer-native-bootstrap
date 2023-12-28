#include "SystemHelper.h"

#include <Shlwapi.h>
#include <sstream>
#include <chrono>

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

	std::wstring value;
	value.resize(dataSize / sizeof(wchar_t));

	retCode = ::RegGetValueW(
		hive,
		path.c_str(),
		key.c_str(),
		RRF_RT_REG_SZ,
		nullptr,
		value.data(),
		&dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		return std::nullopt;
	}

	DWORD stringLengthInWchars = dataSize / sizeof(wchar_t);
	stringLengthInWchars--; // Exclude the NUL written by the Win32 API
	value.resize(stringLengthInWchars);
	return value;
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
	size = ::GetEnvironmentVariableW(key.c_str(), value.data(), size);
	if (!size || size >= value.size()) {
		return std::nullopt;
	}

	value.resize(size);
	return value;
}

void SystemHelper::showErrorMessage(const std::wstring& title, const std::wstring& message, const std::wstring& url) {
	const int result = ::MessageBoxW(
		nullptr,
		message.c_str(),
		title.c_str(),
		MB_ICONWARNING | MB_YESNO
	);

	if (result == IDYES) {
		::ShellExecuteW(nullptr, nullptr, url.c_str(), nullptr, nullptr, SW_SHOW);
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
		return 255;
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

std::wstring SystemHelper::getTempDir()
{
	std::wstring tempDir;
	DWORD size = ::GetTempPath(0, nullptr);

	if (!size) {
		throw std::runtime_error("Failed to get temp path");
	}
		
	tempDir.resize(size + 1);
	size = ::GetTempPath(size + 1, tempDir.data());

	if (!size || size >= tempDir.size()) {
		throw std::runtime_error("Failed to get temp path");
	}
		
	tempDir.resize(size);
	return tempDir;
}

// A windows 7 compatible version of getHostArchitecture, must either be x64 or x86
HostArchitecture::Value getLegacyHostArchitecture() {
#if defined(_M_X64)
	// x64 bin will only run on x64 Windows 7 & 8
	return HostArchitecture::X64;
#else
	BOOL isWow64 = FALSE;

	if (!::IsWow64Process(::GetCurrentProcess(), &isWow64)) {
		return HostArchitecture::Value::UNKNOWN;
	}

	if (isWow64) {
		return HostArchitecture::Value::X64;
	}

	return HostArchitecture::Value::X86;
# endif
}

// https://devblogs.microsoft.com/oldnewthing/20220209-00/?p=106239
// Slightly fun as we are almost always ran though emulation
HostArchitecture::Value SystemHelper::getHostArchitecture() {
#if defined(_M_ARM64)
	// ARM64 bin will only run on ARM64.
	return HostArchitecture::ARM64;
#else
	const auto kernel32Handle = ::GetModuleHandle(TEXT("kernel32.dll"));

	if (kernel32Handle == NULL) {
		throw std::runtime_error("Failed to get kernel32.dll handle, irreversible global collapse imminent!");
	}

	// IsWow64Process2 was added in Windows 10 1709 
	typedef BOOL(WINAPI* IsWow64Process2Func)(HANDLE, USHORT*, USHORT*);
	const IsWow64Process2Func is_wow64_process2 = reinterpret_cast<IsWow64Process2Func>(::GetProcAddress(kernel32Handle, "IsWow64Process2"));

	if (!is_wow64_process2) {
		// Running on older windows.
		return getLegacyHostArchitecture();
	}

	USHORT process_machine = 0;
	USHORT native_machine = IMAGE_FILE_MACHINE_UNKNOWN;
	const auto result = is_wow64_process2(::GetCurrentProcess(), &process_machine, &native_machine);

	if (result == 0) {
		// Error/Unknown
		return HostArchitecture::Value::UNKNOWN;
	}

	switch (native_machine) {
	case IMAGE_FILE_MACHINE_ARM64:
		return HostArchitecture::Value::ARM64;
	case IMAGE_FILE_MACHINE_AMD64:
		return HostArchitecture::Value::X64;
	case IMAGE_FILE_MACHINE_I386:
		return HostArchitecture::Value::X86;
	default:
		return HostArchitecture::Value::UNKNOWN;
	}
# endif
}

long long SystemHelper::getEpochTime()
{
	const auto now = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}
