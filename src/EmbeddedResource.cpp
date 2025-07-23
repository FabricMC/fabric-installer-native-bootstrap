#include "EmbeddedResource.h"

#include <format>

#include <wil/result.h>
#include <wil/resource.h>

EmbeddedResource::EmbeddedResource(WORD resourceId) :
	resourceId(resourceId)
{
}

void EmbeddedResource::ExtractToFile(const std::filesystem::path& path) const
{
	auto module{ ::GetModuleHandle(nullptr) };

	auto intResource{ MAKEINTRESOURCEW(resourceId) };
	auto resource{ THROW_LAST_ERROR_IF_NULL(::FindResourceW(module, intResource, RT_RCDATA)) };
	auto resourceInfo{ THROW_LAST_ERROR_IF_NULL(::LoadResource(module, resource)) };
	auto resourceSize{ ::SizeofResource(module, resource) };
	auto resourceData{ THROW_LAST_ERROR_IF_NULL(::LockResource(resourceInfo)) };

	wil::unique_hfile file{
		::CreateFileW(
			path.c_str(),
			GENERIC_WRITE,
			0,
			nullptr,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		)
	};
	THROW_LAST_ERROR_IF_MSG(!file, "CreateFileW");

	DWORD bytesWritten{ 0 };
	auto success{ ::WriteFile(
		file.get(),
		resourceData,
		resourceSize,
		&bytesWritten,
		nullptr
	) };
	THROW_LAST_ERROR_IF_MSG(!success, "WriteFile");
	THROW_WIN32_IF_MSG(ERROR_HANDLE_EOF, bytesWritten != resourceSize, "Incomplete file write");
}
