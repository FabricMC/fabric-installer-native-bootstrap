#pragma once

#include <Windows.h>
#include <filesystem>

class EmbeddedResource {
public:
	explicit EmbeddedResource(WORD resourceId);

	void ExtractToFile(const std::filesystem::path& path) const;

private:
	WORD resourceId;
};