#include "rpc.h"

#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"
#include "../../engine/logging/logging.h"

#include <cstring>
#include <discord_register.h>
#include <discord_rpc.h>
#include <mutex>

static float last_presence_update = 0.0f;
static int inject_timestamp = -1;
static bool is_initialized = false;

void discord_rpc::update() {
    if (inject_timestamp == -1)
        inject_timestamp = std::time(nullptr);

    if (is_initialized && !settings.miscellaneous.discord_rpc) {
        Discord_ClearPresence();
        Discord_Shutdown();

        is_initialized = false;
        last_presence_update = 0.0f;
    }
    else if (!is_initialized && settings.miscellaneous.discord_rpc) {
        DiscordEventHandlers handlers;

        memset(&handlers, 0, sizeof(handlers));

        // @todo: support joining others' games
        // handlers.joinGame = handleDiscordJoinGame;
        // handlers.joinRequest = handleDiscordJoinRequest;

        Discord_Initialize(XORSTR("558049724675129365"), &handlers, true, nullptr);

        is_initialized = true;
        last_presence_update = 0.0f;
    }

    if (!is_initialized || interfaces::global_vars->real_time - last_presence_update < 15.0f)
        return;

    last_presence_update = interfaces::global_vars->real_time;

    DiscordRichPresence presence;

    memset(&presence, 0, sizeof(presence));

    presence.state = interfaces::engine_client->is_in_game() ? XORSTR("In game") : XORSTR("In main menu");
    presence.largeImageKey = XORSTR("million");
    presence.startTimestamp = inject_timestamp;
    presence.instance = 1;

    Discord_UpdatePresence(&presence);
}