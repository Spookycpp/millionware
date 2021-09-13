#include "../core/hooks/hooks.h"
#include "../core/settings/settings.h"

float __fastcall hooks::get_screen_aspect_ratio(uintptr_t ecx, uintptr_t edx, int width, int height) {

	const auto aspect_ratio = get_screen_aspect_ratio_original(ecx, edx, width, height);

	if (settings.visuals.local.aspect_ratio < 0.01f)
		return aspect_ratio;

	return aspect_ratio * settings.visuals.local.aspect_ratio;
}