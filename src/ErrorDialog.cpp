#include "ErrorDialog.h"
#include "resource.h"

#include <string>
#include <Windows.h>
#include <Commctrl.h>


namespace ErrorDialog {
	namespace {
		constexpr std::wstring_view ERROR_TITLE = L"Fabric Installer";
		constexpr std::wstring_view ERROR_MESSAGE = L"The Fabric Installer could not find a valid Java installation.\n\nPlease visit the the Fabric documentation to find out how to fix this:\n\n<a href=\"\">https://docs.fabricmc.net/players/installing-java/windows</a>";
		constexpr std::wstring_view ERROR_URL = L"https://docs.fabricmc.net/players/installing-java/windows";

		HRESULT CALLBACK dialogCallback(HWND /*hwnd*/, UINT msg, WPARAM /*wParam*/, LPARAM /*lParam*/, LONG_PTR /*refData*/) {
			if (TDN_HYPERLINK_CLICKED != msg)
			{
				return S_OK;
			}


			::ShellExecuteW(nullptr, nullptr, ERROR_URL.data(), nullptr, nullptr, SW_SHOW);
			return S_OK;
		}
	}

	void showErrorDialog() {
		::TASKDIALOGCONFIG config{
			.cbSize = sizeof(::TASKDIALOGCONFIG),
			.hInstance = ::GetModuleHandleW(nullptr),
			.dwFlags = TDF_ENABLE_HYPERLINKS | TDF_SIZE_TO_CONTENT | TDF_ALLOW_DIALOG_CANCELLATION,
			.dwCommonButtons = TDCBF_CLOSE_BUTTON,
			.pszWindowTitle = ERROR_TITLE.data(),
			.pszMainIcon = MAKEINTRESOURCE(IDI_ICON1),
			.pszContent = ERROR_MESSAGE.data(),
			.pfCallback = &dialogCallback
		};

		::TaskDialogIndirect(&config, nullptr, nullptr, nullptr);
	}
}