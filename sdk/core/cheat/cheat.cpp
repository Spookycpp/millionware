#include "cheat.h"

#include "../../engine/input/input.h"
#include "../../engine/logging/logging.h"
#include "../../engine/render/render.h"
#include "../../engine/security/xorstr.h"

#include "../../features/hitchance/hitchance.h"
#include "../../features/miscellaneous/miscellaneous.h"
#include "../../features/visuals/world/world.h"

#include "../../lua/lua_game.hpp"

#include "../../ui/ui.h"

#include "../hooks/hooks.h"

#include "../interfaces/interfaces.h"

#include "../netvars/netvars.h"

#include "../patterns/patterns.h"

#include "../scripting/scripting.h"

#include "../settings/settings.h"

#include "../util/util.h"

#include <discord_rpc.h>
#include <windows.h>

bool cheat::init() {

    if (!std::filesystem::exists(xs(".\\mw")))
        std::filesystem::create_directories(xs(".\\mw"));

    if (!std::filesystem::exists(xs(".\\mw\\scripts")))
        std::filesystem::create_directories(xs(".\\mw\\scripts"));

    if (!std::filesystem::exists(xs(".\\mw\\configs")))
        std::filesystem::create_directories(xs(".\\mw\\configs"));

    if (!patterns::init())
        return false;

    if (!interfaces::init())
        return false;

    //if (interfaces::engine_client->get_engine_build_number() != 13811) {
    //    logging::error(xs("Out of date, report on forums."));
    //    return false;
    //}

    netvars::init();

    const auto device = (IDirect3DDevice9 *) patterns::get_d3d9_device();

    D3DDEVICE_CREATION_PARAMETERS creation_params;

    device->GetCreationParameters(&creation_params);

    render::init(creation_params.hFocusWindow, device);

    input::init(creation_params.hFocusWindow);

    ui::init();

    features::hit_chance::initialize();

    if (!hooks::init())
        return false;

    logging::info(xs("successfully initialized"));
    //logging::info(xs("executing lua scripts..."));

    //lua::init();

    if (!std::filesystem::exists(xs(".\\mw\\configs\\default.json")))
        settings.save();
    else
        settings.load();

    return true;
}

bool cheat::undo() {
    //lua::unload();

    Discord_ClearPresence();
    Discord_Shutdown();

    if (!hooks::undo())
        return false;

    util::undo();

    Sleep(1000);

    render::undo();

    input::undo();

    logging::info(xs("successfully uninitialized"));

    return true;
}
