#include "../core/hooks/hooks.h"

#include "../engine/render/surface.h"

void __fastcall hooks::engine_paint(uintptr_t ecx, uintptr_t edx, int mode) {

    //static bool init_render = false;

    //if (!init_render) {
    //    surface::initialize();
    //    init_render = true;
    //}

    //if (mode & 1) {

        //interfaces::surface->start_drawing();

        //features::visuals::esp::frame();

        //interfaces::surface->finish_drawing();
    //}

    engine_paint_original(ecx, edx, mode);
}
