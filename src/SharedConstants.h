#pragma once

#include <vector>
#include <string>

#if !_WIN32
typedef const wchar_t* LPCWSTR;
#endif

constexpr LPCWSTR ERROR_TITLE = L"Fabric Installer";
constexpr LPCWSTR ERROR_MESSAGE = L"The Fabric Installer could not find a valid Java installation.\n\nWould you like to open the Fabric wiki to find out how to fix this?\n\nURL: https://fabricmc.net/wiki/player:tutorials:java:windows";
constexpr LPCWSTR ERROR_URL = L"https://fabricmc.net/wiki/player:tutorials:java:windows";

static const std::vector<std::wstring> MC_JAVA_NAMES = {
        L"java-runtime-gamma",  // Java 17.0.3
        L"java-runtime-beta",   // Java 17.0.1
        L"java-runtime-alpha",  // Java 16
        L"jre-legacy"           // Java 8
};

namespace {
    void appendPlatform(std::vector<std::wstring>& vec, const std::wstring& platform) {
        for (const auto& name : MC_JAVA_NAMES) {
            std::wstring str;
            str.append(LR"(runtime/)");
            str.append(name);
            str.append(platform);
            str.append(LR"(/bin/)");

            vec.emplace_back(str);
        }
    }

    std::vector<std::wstring> BuildWindows() {
        std::vector<std::wstring> vec;

        appendPlatform(vec, L"windows-x64");
        appendPlatform(vec, L"windows-x86");

        // Java 8 old location
        vec.emplace_back(LR"(runtime/jre-x64/bin/)");
        vec.emplace_back(LR"(runtime/jre-x86/bin/)");
        return vec;
    }

    std::vector<std::wstring> BuildLinux() {
        std::vector<std::wstring> vec;
        appendPlatform(vec, L"linux");
        return vec;
    }
}

static const std::vector<std::wstring> MC_JAVA_PATHS_WIN = BuildWindows();
static const std::vector<std::wstring> MC_JAVA_PATHS_LINUX = BuildLinux();