#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../core/config.hpp"
#include "../core/cheat.hpp"
#include "../utils/render/render.hpp"

void __fastcall hooks::screen_size_changed_hook(uintptr_t ecx, uintptr_t edx, int old_width, int old_height) {
	hooks::screen_size_changed.get_original<decltype(&screen_size_changed_hook)>()(ecx, edx, old_width, old_height);

	render::refresh_fonts();
}