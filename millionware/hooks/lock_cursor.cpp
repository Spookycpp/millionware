#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../gui/gui.hpp"
#include "../utils/render/render.hpp"

void __fastcall hooks::lock_cursor_hook(uintptr_t ecx, uintptr_t edx) {
	if (gui::is_visible()) {
		interfaces::input_system->enable_input(false);
		interfaces::vgui_surface->unlock_cursor();
	}
	else {
		interfaces::input_system->enable_input(true);

		hooks::lock_cursor.get_original<decltype(&lock_cursor_hook)>()(ecx, edx);
	}
}