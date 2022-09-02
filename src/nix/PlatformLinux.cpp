#include "Platform.h"
#include "../SharedConstants.h"
#include <iostream>
#include <locale>
#include <codecvt>
#include <climits>
#include <unistd.h>

using convert_type = std::codecvt_utf8<wchar_t>;
std::wstring_convert<convert_type, wchar_t> converter;
#define STR(ss) converter.to_bytes((ss).str()).c_str()

void ShowMessageBox() {
    bool openHelp = false;

    if (::system("zenity --version") == 0) {
        // zenity installed, use it to display a dialog
        std::wstringstream ss;
        ss << "zenity --question";
        ss << " --text=\"" << ERROR_MESSAGE << "\"";
        ss << " --title=\"" << ERROR_TITLE << "\"";
        int response = ::system(STR(ss));
        if (response == 0) {
            openHelp = true;
        }
    }

    if (!openHelp) {
        return;
    }
    std::wstringstream ss;
    ss << "xdg-open " << ERROR_URL;

    if (::system(STR(ss)) != 0) {
        std::cout << "Failed to open browser";
    }
}

std::filesystem::path GetSelf() {
    char buff[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
        buff[len] = '\0';
        return {buff};
    }

    return "";
}

bool TryLaunchJava(const std::filesystem::path& path) {
    {
        std::wstringstream ss;
        ss << path.c_str();
        ss << " -version";
        if (::system(STR(ss)) != 0) {
            // Invalid java
            return false;
        }
    }

    std::wstringstream ss;
    ss << path.c_str();
    ss << " -jar " << GetSelf();

    // Ignore the return code for this, assume it worked as expected.
    ::system(STR(ss));

    return true;
}