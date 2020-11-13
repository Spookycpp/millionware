#include <mutex>

#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../utils/render.hpp"

std::once_flag initialize_renderer;

void __fastcall engine_paint_hook(uintptr_t ecx, uintptr_t edx, int mode) {
	const auto _ = std::lock_guard(hooks::engine_paint.call_mutex);

	std::call_once(initialize_renderer, render::initialize);

	if (mode & PAINT_MODE_UI_PANELS) {
		interfaces::vgui_surface->start_drawing();

		render::fill_rect_rounded(16, 16, 64, 64, 5, CORNER_ALL, color_t(32));
		render::text(32, 32, e_font::SEGOE_UI_13, color_t(255), "Stinky Niggers");

		interfaces::vgui_surface->finish_drawing();
	}

	return reinterpret_cast<decltype(&engine_paint_hook)>(hooks::engine_paint.original)(ecx, edx, mode);
}
