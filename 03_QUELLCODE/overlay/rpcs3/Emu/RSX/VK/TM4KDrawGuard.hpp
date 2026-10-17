#pragma once
#include <cstdlib>

// Opt-in, process-local diagnostic/correctness guard. No global GPU settings.
inline bool tm4k_draw_guard_requested()
{
	static const bool enabled = []
	{
		const char* value = std::getenv("TM4K_DRAW_GUARD");
		return value && value[0] == '1' && value[1] == '\0';
	}();
	return enabled;
}
