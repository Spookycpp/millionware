#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../core/config.hpp"
#include "../core/cheat.hpp"

float __fastcall hooks::get_screen_aspect_ratio_hook(uintptr_t ecx, uintptr_t edx, int width, int height) {
	return hooks::get_screen_aspect_ratio.get_original<decltype(&get_screen_aspect_ratio_hook)>()(ecx, edx, width, height);
}