#include "Bootstrap.h"

#include <iostream>

#include "../SharedConstants.h"

constexpr LPCWSTR MC_LAUNCH_REG_PATH = LR"(SOFTWARE\Mojang\InstalledProducts\Minecraft Launcher)";
constexpr LPCWSTR MC_LAUNCH_REG_KEY = L"InstallLocation";
static const LPCWSTR UWP_PATH = LR"(\Packages\Microsoft.4297127D64EC6_8wekyb3d8bbwe\LocalCache\Local\)";

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
		std::wstring installPath = minecraftLauncherPath.value();

		// This is weird, on my tests machine the reg key value is just "C:\Program Files (x86)\"
		if (!installPath.ends_with(LR"(Minecraft Launcher\)")) {
			installPath = installPath + LR"(Minecraft Launcher\)";
			std::wcout << "Install path did not appear to be Minecraft, appending guess." << std::endl;
		}

		std::wcout << "Found Minecraft launcher installation path: " << installPath << std::endl;

		for (const LPCWSTR path : MC_JAVA_PATHS) {
			const std::wstring fullPath = installPath + path;
			if (attemptLaunch(fullPath, true)) {
				return true;
			}
		}
	}
	else {
		std::wcout << "Could not find minecraft launcher installation in registry.: " << std::endl;
	}

	// Check %LOCALAPPDATA% for the UWP installer
	if (auto localAppData = systemHelper->getEnvVar(L"LOCALAPPDATA"); localAppData) {
		std::wstring launcherPath = localAppData.value() + UWP_PATH;

		if (systemHelper->dirExists(launcherPath)) {
			for (const LPCWSTR path : MC_JAVA_PATHS) {
				if (attemptLaunch(launcherPath + path, true)) {
					return true;
				}
			}
		}
		else { std::wcout << "Could not find minecraft UWP launcher directory.: " << launcherPath << std::endl; }
	}
	else {
		// Something has gone really wrong :)
		throw std::runtime_error("Failed to get LOCALAPPDATA env var!");
	}

	return false;
}

bool Bootstrap::launchSystemJava() {
	// Check %JAVA_HOME% for system java
	if (auto javaHome = systemHelper->getEnvVar(L"JAVA_HOME"); javaHome) {
		std::wstring path = javaHome.value() + LR"(bin\javaw.exe)";
		if (attemptLaunch(path, true)) {
			return true;
		}
	}
	else {
		std::wcout << "Could not find JAVA_HOME env var" << std::endl;
	}

	std::wcout << "Trying java on the path" << std::endl;
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
	DWORD exit = systemHelper->createProcess({ path, L"-version" });
	if (exit == 0) {
		// -version returned a successful exit code.
		std::wcout << "Found valid java @ (" << path << ")" << std::endl;

		if (systemHelper->createProcess({ path, L"-jar", systemHelper->getBootstrapFilename(), L"-fabricInstallerBootstrap", L"true" }) != 0) {
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