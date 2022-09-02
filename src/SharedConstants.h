#pragma once

#include <vector>
#include <string>

#if !_WIN32
typedef const wchar_t* LPCWSTR;
#endif

constexpr LPCWSTR ERROR_TITLE = L"Fabric Installer";
constexpr LPCWSTR ERROR_MESSAGE = L"The Fabric Installer could not find a valid Java installation.\n\nWould you like to open the Fabric wiki to find out how to fix this?\n\nURL: https://fabricmc.net/wiki/player:tutorials:java:windows";
constexpr LPCWSTR ERROR_URL = L"https://fabricmc.net/wiki/player:tutorials:java:windows";

static const std::vector<LPCWSTR> MC_JAVA_PATHS_WIN = {
	LR"(runtime/java-runtime-gamma/windows-x64/java-runtime-gamma/bin/)", // Java 17.0.3
	LR"(runtime/java-runtime-gamma/windows-x86/java-runtime-gamma/bin/)",
	LR"(runtime/java-runtime-beta/windows-x64/java-runtime-beta/bin/)", // Java 17.0.1
	LR"(runtime/java-runtime-beta/windows-x86/java-runtime-beta/bin/)",
	LR"(runtime/java-runtime-alpha/windows-x64/java-runtime-alpha/bin/)", // Java 16
	LR"(runtime/java-runtime-alpha/windows-x86/java-runtime-alpha/bin/)",
	LR"(runtime/jre-legacy/windows-x64/jre-legacy/bin/)", // Java 8 new location
	LR"(runtime/jre-legacy/windows-x86/jre-legacy/bin/)",
	LR"(runtime/jre-x64/bin/)", // Java 8 old location
	LR"(runtime/jre-x86/bin/)",
};

static const std::vector<LPCWSTR> MC_JAVA_PATHS_LINUX = {
        LR"(runtime/java-runtime-gamma/linux/java-runtime-gamma/bin/)", // Java 17.0.3
        LR"(runtime/java-runtime-beta/linux/java-runtime-beta/bin/)", // Java 17.0.1
        LR"(runtime/java-runtime-alpha/linux/java-runtime-alpha/bin/)", // Java 16
        LR"(runtime/jre-legacy/linux/jre-legacy/bin/)", // Java 8 new location
};