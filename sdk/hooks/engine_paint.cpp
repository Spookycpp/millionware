#include <mutex>

#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"

#include "../engine/hash/hash.h"
#include "../engine/render/surface.h"

#include "../features/nade prediction/nade_prediction.h"
#include "../features/visuals/entities/esp.h"
#include "../features/visuals/world/world.h"

static std::once_flag initialize_renderer;

void __fastcall hooks::engine_paint(uintptr_t ecx, uintptr_t edx, int mode) {
    std::call_once(initialize_renderer, surface::initialize);

    if (mode & 1) {

        interfaces::surface->start_drawing();

        features::visuals::esp::frame();

        features::visuals::world::indicators();

        features::visuals::world::display_spectators();

        features::nade_prediction::on_paint_traverse();

        interfaces::surface->finish_drawing();
    }

    engine_paint_original(ecx, edx, mode);
}
