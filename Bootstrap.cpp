#include "Bootstrap.h"

#include <iostream>
#include <vector>

constexpr LPCWSTR ERROR_TITLE = L"Fabric Installer";
constexpr LPCWSTR ERROR_MESSAGE = L"The Fabric Installer could not find a valid Java installation.\n\nWould you like to open the Fabric wiki to find out how to fix this?\n\nURL: https://fabricmc.net/wiki/player:tutorials:java:windows";
constexpr LPCWSTR ERROR_URL = L"https://fabricmc.net/wiki/player:tutorials:java:windows";

constexpr LPCWSTR MC_LAUNCH_REG_PATH = LR"(SOFTWARE\Mojang\InstalledProducts\Minecraft Launcher)";
constexpr LPCWSTR MC_LAUNCH_REG_KEY = L"InstallLocation";

constexpr LPCWSTR UWP_LAUNCH_PATH = LR"(\Packages\Microsoft.4297127D64EC6_8wekyb3d8bbwe\LocalCache\Local\Microsoft\WritablePackageRoot)";

static const std::vector<LPCWSTR> MC_JAVA_PATHS = {
		LR"(runtime\java-runtime-beta\windows-x64\java-runtime-beta\bin\javaw.exe)", // Java 17
		LR"(runtime\java-runtime-beta\windows-x86\java-runtime-beta\bin\javaw.exe)",
		LR"(runtime\java-runtime-alpha\windows-x64\java-runtime-alpha\bin\javaw.exe)", // Java 16
		LR"(runtime\java-runtime-alpha\windows-x86\java-runtime-alpha\bin\javaw.exe)",
		LR"(runtime\jre-legacy\windows-x64\jre-legacy\bin\javaw.exe)", // Java 8 new location
		LR"(runtime\jre-legacy\windows-x86\jre-legacy\bin\javaw.exe)",
		LR"(runtime\jre-x64\bin\javaw.exe)", // Java 8 old location
		LR"(runtime\jre-x86\bin\javaw.exe)",
};

Bootstrap::Bootstrap(const std::shared_ptr<ISystemHelper>& systemHelper) : systemHelper(systemHelper) {}

void Bootstrap::launch() {
	bool launched;

	launched = launchMinecraftLauncher();

	if (!launched) {
		launched = launchSystemJava();
	}

	if (!launched) {
		showErrorMessage();
	}
}

bool Bootstrap::launchMinecraftLauncher() {
	if (auto minecraftLauncherPath = systemHelper->getRegValue(HKEY_CURRENT_USER, MC_LAUNCH_REG_PATH, MC_LAUNCH_REG_KEY); minecraftLauncherPath) {
		std::wcout << "Found Minecraft launcher path: " << minecraftLauncherPath.value() << std::endl;

		for (const LPCWSTR path : MC_JAVA_PATHS) {
			const std::wstring fullPath = minecraftLauncherPath.value() + path;
			if (attemptLaunch(fullPath, true)) {
				return true;
			}
		}
	}
	else {
		std::wcout << "Could not find minecraft launcher installation directory in registry.: " << minecraftLauncherPath.value() << std::endl;
	}

	// Check %LOCALAPPDATA% for the UWP installer
	if (auto localAppData = systemHelper->getEnvVar(L"LOCALAPPDATA"); localAppData) {
		std::wstring launcherPath = localAppData.value() + UWP_LAUNCH_PATH;

		if (systemHelper->dirExists(launcherPath)) {
			for (const LPCWSTR path : MC_JAVA_PATHS) {
				if (attemptLaunch(launcherPath + path, true)) {
					return true;
				}
			}
		}
		else {
			std::wcout << "Could not find minecraft UWP launcher directory.: " << launcherPath << std::endl;
		}
	}
	else {
		// Something has gone really wrong :)
		throw std::runtime_error("Failed to get LOCALAPPDATA env var!");
	}

	return false;
}

bool Bootstrap::launchSystemJava() {
	// Check %JAVA_HOME% for system java
	if (auto localAppData = systemHelper->getEnvVar(L"JAVA_HOME"); localAppData) {
		std::wstring path = localAppData.value() + UWP_LAUNCH_PATH;
		if (attemptLaunch(path, true)) {
			return true;
		}
	}
	else {
		std::wcout << "Could not find JAVA_HOME env var" << std::endl;
	}

	std::wcout << "Trying system java" << std::endl;
	return attemptLaunch(L"javaw.exe", false);
}

void Bootstrap::showErrorMessage() {
	std::wcout << "Failed to launch showing error dialog" << std::endl;
	systemHelper->showErrorMessage(ERROR_TITLE, ERROR_MESSAGE, ERROR_URL);
}

bool Bootstrap::attemptLaunch(const std::wstring& path, bool checkExists) {
	if (checkExists) {
		if (!systemHelper->fileExists(path)) {
			std::wcout << "Java path (" << path << ") does not exist" << std::endl;
			return false;
		}
	}

	std::wcout << "Testing for valid java @ (" << path << ")" << std::endl;
	DWORD exit = systemHelper->createProcess(path, { L"-version" });
	if (exit == 0) {
		// -version returned a successful exit code.
		std::wcout << "Found valid java @ (" << path << ")" << std::endl;

		if (systemHelper->createProcess(path, { L"-jar", systemHelper->getBootstrapFilename() }) != 0) {
			// The installer returned a none 0 exit code, meaning that most likely the installer crashed.
			throw std::runtime_error("Installer returned a none 0 exit code");
		}

		return true;
	}
	else {
		std::wcout << "Java @ (" << path << ") returned an exit code of: " << std::to_wstring(exit) << std::endl;
	}

	return false;
}