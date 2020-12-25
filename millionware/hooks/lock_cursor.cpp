#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../gui/gui.hpp"
#include "../utils/render.hpp"

void __fastcall hooks::lock_cursor_hook(uintptr_t ecx, uintptr_t edx) {
	const auto _ = std::lock_guard(hooks::lock_cursor.call_mutex);

	if (gui::is_visible()) {
		interfaces::input_system->enable_input(false);
		interfaces::vgui_surface->unlock_cursor();
	}
	else {
		interfaces::input_system->enable_input(true);

		reinterpret_cast<decltype(&lock_cursor_hook)>(hooks::lock_cursor.original)(ecx, edx);
	}
}
