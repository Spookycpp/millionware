#include <Windows.h>

#include <cstdint>

#include "../core/hooks.hpp"

long __fastcall present_hook(const uintptr_t ecx, const uintptr_t edx, const RECT* source_rect, const RECT* dest_rect, const HWND dest_window_override, const RGNDATA* dirty_region) {
	return reinterpret_cast<decltype(&present_hook)>(hooks::original_present)(ecx, edx, source_rect, dest_rect, dest_window_override, dirty_region);
}
