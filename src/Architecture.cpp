#include "Architecture.h"

std::wstring Architecture::AsString(const Value& arch)
{
	switch (arch) {
	case X64:
		return L"x64";
	case ARM64:
		return L"arm64";
	case X86:
		return L"x86";
	case UNKNOWN:
	default:
		return L"unknown";
	}
}
