#include "SystemHelper.h"

#include <Shlwapi.h>
#include <sstream>
#include <chrono>
#include <wil/registry.h>
#include <wil/win32_helpers.h>

std::optional<std::wstring> SystemHelper::getRegValue(HKEY hive, const std::wstring& path, const std::wstring& key) const {
	return wil::reg::try_get_value_string(hive, path.c_str(), key.c_str());
}

std::optional<std::wstring> SystemHelper::getEnvVar(const std::wstring& key) const {
	auto value{ wil::TryGetEnvironmentVariableW(key.c_str()) };

	if (value) {
		return wil::str_raw_ptr(value);
	}

	return std::nullopt;
}

void SystemHelper::showErrorMessage(const std::wstring& title, const std::wstring& message, const std::wstring& url) const {
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

DWORD SystemHelper::createProcess(const std::vector<std::wstring>& args) const {
	STARTUPINFOW info{ 0 };
	wil::unique_process_information processInfo;

	std::wstringstream cls;
	for (auto& arg : args) {
		cls << L"\"" << arg << L"\" ";
	}

	// Must copy as CreateProcessW can mutate this!!
	std::wstring commandLine = cls.str();

	// Create the child process
	THROW_LAST_ERROR_IF(!::CreateProcessW(
		nullptr,
		commandLine.data(),
		nullptr,
		nullptr,
		false,
		CREATE_DEFAULT_ERROR_MODE,
		nullptr,
		nullptr,
		&info,
		&processInfo)
	);

	// Wait for exit
	::WaitForSingleObject(processInfo.hProcess, INFINITE);

	// Read exit code
	DWORD exitCode;
	::GetExitCodeProcess(processInfo.hProcess, &exitCode);

	return exitCode;

}

bool SystemHelper::fileExists(const std::wstring& path) const {
	const auto attributes = ::GetFileAttributesW(path.c_str());
	return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool SystemHelper::dirExists(const std::wstring& path) const {
	const auto attributes = ::GetFileAttributesW(path.c_str());
	return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}

std::wstring SystemHelper::getBootstrapFilename() const {
	wchar_t moduleFileName[MAX_PATH] = { 0 };
	::GetModuleFileNameW(nullptr, moduleFileName, MAX_PATH);
	return moduleFileName;
}

std::wstring SystemHelper::getTempDir() const {
	std::wstring tempDir;
	DWORD size = ::GetTempPathW(0, nullptr);
	THROW_LAST_ERROR_IF(!size);

	tempDir.resize((size_t)size + 1);
	size = ::GetTempPathW(size + 1, tempDir.data());
	THROW_LAST_ERROR_IF(!size || size >= tempDir.size());

	tempDir.resize(size);
	return tempDir;
}

// A windows 7 compatible version of getHostArchitecture, must either be x64 or x86
Architecture::Value getLegacyHostArchitecture() {
#if defined(_M_X64)
	// x64 bin will only run on x64 Windows 7 & 8
	return Architecture::X64;
#else
	BOOL isWow64 = FALSE;

	if (!::IsWow64Process(::GetCurrentProcess(), &isWow64)) {
		return Architecture::Value::UNKNOWN;
	}

	if (isWow64) {
		return Architecture::Value::X64;
	}

	return Architecture::Value::X86;
# endif
}

// https://devblogs.microsoft.com/oldnewthing/20220209-00/?p=106239
// Slightly fun as we are almost always ran though emulation
Architecture::Value SystemHelper::getHostArchitecture() const {
#if defined(_M_ARM64)
	// ARM64 bin will only run on ARM64.
	return Architecture::ARM64;
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
		return Architecture::Value::UNKNOWN;
	}

	switch (native_machine) {
	case IMAGE_FILE_MACHINE_ARM64:
		return Architecture::Value::ARM64;
	case IMAGE_FILE_MACHINE_AMD64:
		return Architecture::Value::X64;
	case IMAGE_FILE_MACHINE_I386:
		return Architecture::Value::X86;
	default:
		return Architecture::Value::UNKNOWN;
	}
# endif
}

int64_t SystemHelper::getEpochTime() const {
	const auto now = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}
