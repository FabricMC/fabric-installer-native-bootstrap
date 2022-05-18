#include "Bootstrap.h"
#include "SystemHelper.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapsetinformation
	HeapSetInformation(GetProcessHeap(), HeapEnableTerminationOnCorruption, NULL, 0);
	// We dont want to try and load DLLs from the current directory (Quite often the downloads dir)
	SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32);

	const auto systemHelper = std::make_shared<SystemHelper>();

	auto bs = Bootstrap(systemHelper);
	bs.launch();

	return 0;
}
