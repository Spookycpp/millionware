#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../core/config.hpp"
#include "../core/cheat.hpp"

float __fastcall hooks::get_screen_aspect_ratio_hook(uintptr_t ecx, uintptr_t edx, int width, int height) {

	const auto aspect_ratio = hooks::get_screen_aspect_ratio.get_original<decltype(&get_screen_aspect_ratio_hook)>()(ecx, edx, width, height);

	if (config::get<float>(FNV_CT("visuals.other.general.aspect_ratio")) < 0.01f)
		return aspect_ratio;

	return aspect_ratio * config::get<float>(FNV_CT("visuals.other.general.aspect_ratio"));
}