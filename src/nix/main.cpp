#include <iostream>

#include "Platform.h"
#include "../SharedConstants.h"

int main(int argc, char *argv[])
{
    const auto home = std::filesystem::path(getenv("HOME"));

#if __linux__
    const std::filesystem::path runtime = home / ".var" / "app" / "com.mojang.Minecraft" / ".minecraft";

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
    // TODO MacOS impl.
#endif

    const auto JAVA_HOME = std::filesystem::path(getenv("JAVA_HOME"));
    // TODO try JAVA_HOME
    // TODO try path.

    ShowMessageBox();
}