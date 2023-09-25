#include "Bootstrap.h"
#include "SystemHelper.h"

#include <stdexcept>
#include <iostream>

_Use_decl_annotations_ int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(pCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	// We dont want to try and load DLLs from the current directory (Quite often the downloads dir)
	::SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32);
	// https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapsetinformation
	::HeapSetInformation(::GetProcessHeap(), HeapEnableTerminationOnCorruption, NULL, 0);

	const auto systemHelper = std::make_shared<SystemHelper>();

	auto bs = Bootstrap(systemHelper);

	try {
		bs.launch();
	} catch (const std::runtime_error& error) {
		std::cout << error.what() << std::endl;
		return 1;
	}

	return 0;
}
