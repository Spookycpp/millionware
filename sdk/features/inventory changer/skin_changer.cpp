#include "skin_changer.h"
#include "item_definitions.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"

#include "../../source engine/client_class.h"
#include "../../source engine/econ_item.h"

#include <vector>

namespace features::skin_changer {

    std::vector<c_econ_item *> items = {};

    create_client_class_fn get_wearable_create_fn();

    void on_frame_stage_notify(const e_client_frame_stage current_stage) {

        if (current_stage != e_client_frame_stage::FRAME_STAGE_NET_UPDATE_POSTDATAUPDATE_START)
            return;

        player_info_t info;

        if (!interfaces::engine_client->get_player_info(cheat::local_player->get_networkable()->index(), info))
            return;

        apply_glove_skins(info);
        apply_weapon_skins(info);
    }

    void apply_weapon_skins(const player_info_t &player_info) {
    }

    void apply_glove_skins(const player_info_t &player_info) {
    }

    void update_inventory() {
    }

    void remove_inventory() {
    }

    void update_hud_icons() {
    }
} // namespace features::skin_changer