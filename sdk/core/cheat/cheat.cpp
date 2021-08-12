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
    if (!patterns::init())
        return false;

    if (!interfaces::init())
        return false;

    // on csgo updates this'll change, anti-crack method i guess
    // if the cheat doesn't inject open your console in csgo
    // and type "version" & grab the "Exe version" value
    // remove the dots & you have the build number
    // big bash bosh
    if (interfaces::engine_client->get_engine_build_number() != 13798) {
        logging::info(xs("tell laine 'big bash bosh'"));
        return false;
    }

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
    logging::info(xs("executing lua scripts..."));

    lua::init();

    return true;
}

bool cheat::undo() {

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
