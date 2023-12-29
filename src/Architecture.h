#pragma once

#include <string>

namespace Architecture
{
	// Ordered to ensure that we only check for JVMs that will run on the host arch.
	// On an x64 host only check for x64 and x86 as arm will never run.
	// On x86 there is no need to try any other arch as it wont run.
	enum Value {
		UNKNOWN,
		ARM64,
		X64,
		X86,
	};

	constexpr Value VALUES[] = { UNKNOWN, ARM64, X64, X86 };

	std::wstring AsString(const Value& arch);
};

