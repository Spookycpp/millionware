#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../utils/render.hpp"

void __fastcall screen_size_changed_hook(uintptr_t ecx, uintptr_t edx, int old_width, int old_height) {
	const auto _ = std::lock_guard(hooks::screen_size_changed.call_mutex);

	reinterpret_cast<decltype(&screen_size_changed_hook)>(hooks::screen_size_changed.original)(ecx, edx, old_width, old_height);

	render::refresh_fonts();
}
