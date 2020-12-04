#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../utils/render.hpp"
#include "../core/config.hpp"
#include "../core/cheat.hpp"

float __fastcall hooks::get_screen_aspect_ratio_hook(uintptr_t ecx, uintptr_t edx, int width, int height) {
	const auto _ = std::lock_guard(hooks::get_screen_aspect_ratio.call_mutex);

	const auto aspect_ratio = reinterpret_cast<decltype(&get_screen_aspect_ratio_hook)>(hooks::get_screen_aspect_ratio.original)(ecx, edx, width, height);

	if (config::get<float>(FNV_CT("misc.other.aspect_ratio")) < 0.01f)
		return aspect_ratio;

	return aspect_ratio * config::get<float>(FNV_CT("misc.other.aspect_ratio"));
}