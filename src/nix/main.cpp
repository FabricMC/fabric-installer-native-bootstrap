#include <iostream>

#include "Platform.h"
#include "../SharedConstants.h"

int main([[maybe_unused]] int argc,[[maybe_unused]]  char *argv[])
{
    const auto HOME = std::filesystem::path(getenv("HOME"));

#if __linux__
    const std::filesystem::path runtime = HOME / ".var" / "app" / "com.mojang.Minecraft" / ".minecraft";

    if (std::filesystem::exists(runtime)) {
        for (const LPCWSTR path : MC_JAVA_PATHS_LINUX) {
            const std::filesystem::path javaExe = runtime / path / "java";

            std::cout << "Searching for Java at: " << javaExe << std::endl;

            if (std::filesystem::exists(javaExe)) {
                std::cout << "Trying to launch with: " << javaExe << std::endl;

                if (TryLaunchJava(javaExe)) {
                    // Successful launch!
                    return 0;
                }
            }
        }
    }
#elif __APPLE__
    // TODO MacOS impl?
#endif
    const auto JAVA_HOME = getenv("JAVA_HOME");

    if (JAVA_HOME != nullptr) {
        if (TryLaunchJava(std::filesystem::path(JAVA_HOME) / "bin" / "java")) {
            // Successful launch using JAVA_HOME
            return 0;
        }
    }

    if (TryLaunchJava("java")) {
        // Successful launch using java on the path!
        return 0;
    }

    // Unable to launch, show message box.
    ShowMessageBox();
    return 1;
}