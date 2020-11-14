#include <mutex>

#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../menu/menu.hpp"
#include "../thirdparty/xorstr/xorstr.hpp"
#include "../utils/render.hpp"

std::once_flag initialize_renderer;

void __fastcall engine_paint_hook(uintptr_t ecx, uintptr_t edx, int mode) {
	const auto _ = std::lock_guard(hooks::engine_paint.call_mutex);

	std::call_once(initialize_renderer, render::initialize);

	if (mode & PAINT_MODE_UI_PANELS) {
		interfaces::vgui_surface->start_drawing();

		menu::frame();

		interfaces::vgui_surface->finish_drawing();
	}

	return reinterpret_cast<decltype(&engine_paint_hook)>(hooks::engine_paint.original)(ecx, edx, mode);
}
