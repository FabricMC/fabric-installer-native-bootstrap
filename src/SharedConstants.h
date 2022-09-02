#pragma once

#include <vector>
#include <string>

#if !_WIN32
typedef const wchar_t* LPCWSTR;
#endif

constexpr LPCWSTR ERROR_TITLE = L"Fabric Installer";
constexpr LPCWSTR ERROR_MESSAGE = L"The Fabric Installer could not find a valid Java installation.\n\nWould you like to open the Fabric wiki to find out how to fix this?\n\nURL: https://fabricmc.net/wiki/player:tutorials:java:windows";
constexpr LPCWSTR ERROR_URL = L"https://fabricmc.net/wiki/player:tutorials:java:windows";

static const std::vector<LPCWSTR> MC_JAVA_PATHS = {
	LR"(runtime\java-runtime-gamma\windows-x64\java-runtime-gamma\bin\javaw.exe)", // Java 17.0.3
	LR"(runtime\java-runtime-gamma\windows-x86\java-runtime-gamma\bin\javaw.exe)",
	LR"(runtime\java-runtime-beta\windows-x64\java-runtime-beta\bin\javaw.exe)", // Java 17.0.1
	LR"(runtime\java-runtime-beta\windows-x86\java-runtime-beta\bin\javaw.exe)",
	LR"(runtime\java-runtime-alpha\windows-x64\java-runtime-alpha\bin\javaw.exe)", // Java 16
	LR"(runtime\java-runtime-alpha\windows-x86\java-runtime-alpha\bin\javaw.exe)",
	LR"(runtime\jre-legacy\windows-x64\jre-legacy\bin\javaw.exe)", // Java 8 new location
	LR"(runtime\jre-legacy\windows-x86\jre-legacy\bin\javaw.exe)",
	LR"(runtime\jre-x64\bin\javaw.exe)", // Java 8 old location
	LR"(runtime\jre-x86\bin\javaw.exe)",
};