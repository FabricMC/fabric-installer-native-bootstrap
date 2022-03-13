#include "Bootstrap.h"
#include "SystemHelper.h"

int main() {
	const auto systemHelper = std::make_shared<SystemHelper>();

	auto bs = Bootstrap(systemHelper);
	bs.launch();

	return 0;
}
