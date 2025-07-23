#include "Bootstrap.h"
#include "SystemHelper.h"
#include "Logger.h"

#include <stdexcept>

_Use_decl_annotations_ int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(pCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	const auto kernel32Handle = ::GetModuleHandle(TEXT("kernel32.dll"));

	if (kernel32Handle == NULL) {
		return -1;
	}

	// Not supported on all versions of Windows 7
	typedef BOOL(WINAPI* SetDefaultDllDirectoriesFunc)(DWORD);
	const SetDefaultDllDirectoriesFunc set_default_dll_directries = reinterpret_cast<SetDefaultDllDirectoriesFunc>(::GetProcAddress(kernel32Handle, "SetDefaultDllDirectories"));

	if (set_default_dll_directries) {
		// We dont want to try and load DLLs from the current directory (Quite often the downloads dir)
		set_default_dll_directries(LOAD_LIBRARY_SEARCH_SYSTEM32);
	}

	// https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapsetinformation
	::HeapSetInformation(::GetProcessHeap(), HeapEnableTerminationOnCorruption, NULL, 0);

	SystemHelper systemHelper;
	Logger logger{ systemHelper };

	logger.log(L"Fabric launcher native bootstrap log:");

	auto bs = Bootstrap(systemHelper, logger);

	try {
		bs.launch();
	}
	catch (const std::runtime_error& error) {
		logger.log(L"A runtime error occured:");
		logger.log(error.what());
		return 1;
	}

	return 0;
}
