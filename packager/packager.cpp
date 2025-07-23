#include <print>
#include <filesystem>
#include <fstream>
#include <wil/resource.h>
#include <Windows.h>

namespace {
	constexpr auto IDI_EMBEDDED_JAR = 201;

	std::vector<char> readFile(const std::filesystem::path& path) {
		wil::unique_hfile file{
			::CreateFileW(
				path.c_str(),
				GENERIC_READ,
				FILE_SHARE_READ,
				nullptr,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				nullptr
			)
		};
		THROW_LAST_ERROR_IF_MSG(!file, "CreateFileW");

		LARGE_INTEGER fileSize{};
		THROW_LAST_ERROR_IF(::GetFileSizeEx(file.get(), &fileSize) == 0);
		THROW_WIN32_IF(ERROR_FILE_TOO_LARGE, fileSize.QuadPart > SIZE_MAX);

		std::vector<char> buffer(static_cast<size_t>(fileSize.QuadPart));
		DWORD bytesRead = 0;
		THROW_LAST_ERROR_IF(!::ReadFile(file.get(), buffer.data(), static_cast<DWORD>(buffer.size()), &bytesRead, nullptr));
		THROW_WIN32_IF(ERROR_HANDLE_EOF, bytesRead != buffer.size());

		return buffer;
	}

	void updateResource(const std::filesystem::path& exe, const std::vector<char>& data, WORD resourceId) {
		HANDLE handle = THROW_LAST_ERROR_IF_NULL(::BeginUpdateResourceW(exe.c_str(), false));

		bool success = false;
		auto _ = wil::scope_exit([&]() noexcept { ::EndUpdateResourceW(handle, !success); });

		auto intResource{ MAKEINTRESOURCE(resourceId) };
		THROW_LAST_ERROR_IF(!::UpdateResourceW(handle, RT_RCDATA, intResource, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPVOID)data.data(), static_cast<DWORD>(data.size())));

		success = true;
	}

	void packageInstaller(const std::filesystem::path& bootstrap, std::filesystem::path& installer) {
		THROW_WIN32_IF_MSG(ERROR_FILE_NOT_FOUND, !std::filesystem::exists(bootstrap), "Boostrap not found");
		THROW_WIN32_IF_MSG(ERROR_FILE_NOT_FOUND, !std::filesystem::exists(installer), "Installer not found");

		auto installerBytes = readFile(installer);
		updateResource(bootstrap, installerBytes, IDI_EMBEDDED_JAR);

		std::print("Done, {} updated to embed {}\n", bootstrap.string(), installer.string());
	}
}

int wmain(int argc, wchar_t* argv[])
{
	if (argc != 3) {
		std::print("Usage: packager <bootstrap_exe> <embedded_installer>\n");
		return 1;
	}

	std::filesystem::path bootstrap{ argv[1] };
	std::filesystem::path installer{ argv[2] };

	std::print("Packager: {} Installer: {}\n", bootstrap.string(), installer.string());

	try {
		packageInstaller(bootstrap, installer);
	}
	catch (const std::exception& error) {
		std::print("Packager failed: {}\n", error.what());
		throw;
	}

	return 0;
}