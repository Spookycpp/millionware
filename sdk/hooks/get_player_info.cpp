#include "../core/hooks/hooks.h"
#include "../core/settings/settings.h"
#include "../engine/security/xorstr.h"

bool __fastcall hooks::get_player_info(c_engine_client *ecx, uintptr_t edx, int ent_index, player_info_t &buffer) {
    const auto result = get_player_info_original(ecx, edx, ent_index, buffer);

    if (!result)
        return false;

    if (settings.miscellaneous.player_privacy && !buffer.fake_player && !buffer.hltv) {
        buffer.xuid = 0;

        memset(buffer.name, 0, sizeof buffer.name);

        sprintf_s(buffer.name, xs("Player %d"), buffer.user_id);
    }

    return true;
}
