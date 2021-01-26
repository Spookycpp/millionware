#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../menu/menu.hpp"
#include "../utils/input/input.hpp"
#include "../utils/render/render.hpp"

static std::once_flag initialize_renderer;

void __fastcall hooks::engine_paint_hook(uintptr_t ecx, uintptr_t edx, int mode) {
	std::call_once(initialize_renderer, render::initialize);

	if (mode & PAINT_MODE_UI_PANELS) {
		interfaces::vgui_surface->start_drawing();

		input::update();

		menu::frame();

		interfaces::vgui_surface->finish_drawing();
	}

	hooks::engine_paint.get_original<decltype(&engine_paint_hook)>()(ecx, edx, mode);
}