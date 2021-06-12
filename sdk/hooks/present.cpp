#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../engine/render/render.h"
#include "../engine/security/xorstr.h"
#include "../features/visuals/players/esp.h"
#include "../features/visuals/world/world.h"
#include "../ui/ui.h"
#include "../features/nade prediction/nade_prediction.h"

#include "../lua/lua_game.hpp"

static int active_tab = 0;

long __stdcall hooks::present(IDirect3DDevice9 *device, RECT *source_rect, RECT *dest_rect, HWND dest_window_override, RGNDATA *dirty_region) {
    render::begin();

    features::visuals::esp::frame();

    features::visuals::world::indicators();

    features::nade_prediction::on_paint_traverse();

    ui::frame();

    lua::callbacks::draw();

    render::finish();

    return present_original(device, source_rect, dest_rect, dest_window_override, dirty_region);
}
