#pragma once

#include "../../source engine/client_dll.h"
#include "../../source engine/engine_client.h"

namespace features::skin_changer {
    void on_frame_stage_notify(e_client_frame_stage current_stage);

    void update_inventory();
    void remove_inventory();
    void update_hud_icons();

    void apply_weapon_skins(const player_info_t &player_info);
    void apply_glove_skins(const player_info_t &player_info);
}; // namespace features::skin_changer