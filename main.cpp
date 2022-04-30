#include "Bootstrap.h"
#include "SystemHelper.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	const auto systemHelper = std::make_shared<SystemHelper>();

	auto bs = Bootstrap(systemHelper);
	bs.launch();

	return 0;
}
