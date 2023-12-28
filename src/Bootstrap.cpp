#include "Bootstrap.h"

#include <iostream>
#include <vector>
#include <map>
#include <sstream>

namespace {
	constexpr LPCWSTR ERROR_TITLE = L"Fabric Installer";
	constexpr LPCWSTR ERROR_MESSAGE = L"The Fabric Installer could not find a valid Java installation.\n\nWould you like to open the Fabric wiki to find out how to fix this?\n\nURL: https://fabricmc.net/wiki/player:tutorials:java:windows";
	constexpr LPCWSTR ERROR_URL = L"https://fabricmc.net/wiki/player:tutorials:java:windows";

	constexpr LPCWSTR MC_LAUNCH_REG_PATH = LR"(SOFTWARE\Mojang\InstalledProducts\Minecraft Launcher)";
	constexpr LPCWSTR MC_LAUNCH_REG_KEY = L"InstallLocation";

	constexpr LPCWSTR UWP_PATH = LR"(\Packages\Microsoft.4297127D64EC6_8wekyb3d8bbwe\LocalCache\Local\)";

	// Find these here: https://piston-meta.mojang.com/v1/products/java-runtime/2ec0cc96c44e5a76b9c8b7c39df7210883d12871/all.json
	static const std::vector<LPCWSTR> JAVA_NAMES = {
		L"java-runtime-gamma", // Java 17.0.8
		L"java-runtime-beta", // Java 17.0.1
		L"java-runtime-alpha", // Java 16
		L"jre-legacy", // Java 8
	};
}

Bootstrap::Bootstrap(ISystemHelper& systemHelper, Logger& logger) : systemHelper(systemHelper), logger(logger) {}

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
	const HostArchitecture::Value hostArch = systemHelper.getHostArchitecture();
	logger.log(L"Host archiecture: " + HostArchitecture::AsString(hostArch));

	const auto javaPaths = getMinecraftJavaPaths(hostArch);

	if (auto minecraftLauncherPath = systemHelper.getRegValue(HKEY_CURRENT_USER, MC_LAUNCH_REG_PATH, MC_LAUNCH_REG_KEY); minecraftLauncherPath) {
		std::wstring installPath = minecraftLauncherPath.value();

		// This is weird, on my tests machine the reg key value is just "C:\Program Files (x86)\"
		if (!installPath.ends_with(LR"(Minecraft Launcher\)")) {
			installPath = installPath + LR"(Minecraft Launcher\)";
		}

		logger.log(L"Minecraft launcher installation path: " + installPath);

		for (const auto& path : javaPaths) {
			const std::wstring fullPath = installPath + path;
			if (attemptLaunch(fullPath, true)) {
				return true;
			}
		}
	}
	else {
		logger.log(L"Failed to find minecraft launcher installation directory in registry.");
	}

	// Check %LOCALAPPDATA% for the UWP installer
	if (auto localAppData = systemHelper.getEnvVar(L"LOCALAPPDATA"); localAppData) {
		std::wstring launcherPath = localAppData.value() + UWP_PATH;

		if (systemHelper.dirExists(launcherPath)) {
			for (const auto& path : javaPaths) {
				if (attemptLaunch(launcherPath + path, true)) {
					return true;
				}
			}
		}
		else { 
			logger.log(L"Did not find Minecraft UWP at " + launcherPath);
		}
	}
	else {
		// Something has gone really wrong :)
		throw std::runtime_error("Failed to get LOCALAPPDATA env var!");
	}

	logger.log(L"Failed to launch using Java from the Minecraft Launcher");

	return false;
}

bool Bootstrap::launchSystemJava() {
	// Check %JAVA_HOME% for system java
	if (auto javaHome = systemHelper.getEnvVar(L"JAVA_HOME"); javaHome) {
		logger.log(L"JAVA_HOME = " + javaHome.value());
		std::wstring path = javaHome.value() + LR"(bin\javaw.exe)";
		if (attemptLaunch(path, true)) {
			return true;
		}
	}
	else {
		logger.log("Could not find JAVA_HOME env var");
	}

	logger.log("Trying Java from the system path");
	return attemptLaunch(L"javaw.exe", false);
}

void Bootstrap::showErrorMessage() {
	logger.log("Failed to launch showing error dialog");
	systemHelper.showErrorMessage(ERROR_TITLE, ERROR_MESSAGE, ERROR_URL);
}

bool Bootstrap::attemptLaunch(const std::wstring& path, bool checkExists) {
	if (checkExists) {
		if (!systemHelper.fileExists(path)) {
			logger.log(L"Java path (" + path + L") does not exist");
			return false;
		}
	}

	logger.log(L"Testing for valid java at (" + path + L")");
	DWORD exit = systemHelper.createProcess({ path, L"-version" });
	if (exit != 0) {
		logger.log(L"Java at (" + path + L") returned an exit code of: " + std::to_wstring(exit));
		return false;
	}

	logger.log(L"Found valid Java path (" + path + L")");

	exit = systemHelper.createProcess({ path, L"-jar", systemHelper.getBootstrapFilename(), L"-fabricInstallerBootstrap", L"true" });
	if (exit != 0) {
		// The installer returned a none 0 exit code, meaning that most likely the installer crashed.
		logger.log(L"Installer failed or crashed, exit code: " + std::to_wstring(exit));
		throw std::runtime_error("Installer returned a none 0 exit code: " + exit);
	}

	return true;
}

// Return all of the possible java paths, starting with the newest on the host platform, down to the oldest on the none host platforms.
const std::vector<std::wstring> Bootstrap::getMinecraftJavaPaths(const HostArchitecture::Value& hostArch) {
	std::vector<std::wstring> paths;

	for (const HostArchitecture::Value& arch : HostArchitecture::VALUES) {
		if (arch < hostArch || arch == HostArchitecture::UNKNOWN) {
			// Skip arches that the host does not support.
			// E.g: On x64 there is no need to go looking for arm64 JDKs as its never going to run.
			logger.log(L"Arch not supported: " + HostArchitecture::AsString(arch));
			continue;
		}

		std::wstring javaName;

		switch (arch) {
		case HostArchitecture::X64:
			javaName = L"windows-x64";
			break;
		case HostArchitecture::ARM64:
			javaName = L"windows-arm64";
			break;
		case HostArchitecture::X86:
			javaName = L"windows-x86";
			break;
		default:
			continue;
		}

		for (const LPCWSTR& name : JAVA_NAMES) {
			std::wstringstream buffer;
			// runtime\java-runtime-gamma\windows-x64\java-runtime-gamma\bin\javaw.exe
			buffer << LR"(runtime\)" << name << LR"(\)" << javaName << LR"(\)" << name << LR"(\bin\javaw.exe)";
			paths.push_back(buffer.str());

			logger.log(L"Adding possible java path: " + buffer.str());
		}
	}

	return paths;
}