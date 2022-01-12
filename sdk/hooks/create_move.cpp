#include <mutex>
#include <intrin.h>


#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/settings/settings.h"

#include "../engine/debug/debug_overlay.h"
#include "../engine/math/math.h"

#include "../features/assistance/lagcompensation/lag_comp.h"
#include "../features/assistance/legitbot.h"
#include "../features/assistance/triggerbot/triggerbot.h"
#include "../features/fake ping/fake_ping.h"
#include "../features/miscellaneous/miscellaneous.h"
#include "../features/movement/engine prediction/engine_prediction.h"
#include "../features/movement/movement.h"
#include "../features/nade prediction/nade_prediction.h"
#include "../features/visuals/world/world.h"

#include "../lua/lua_game.hpp"

bool __fastcall hooks::create_move(c_client_mode *ecx, uintptr_t edx, float frame_time, c_user_cmd *user_cmd) {

    PROFILE_WITH(create_move);

    if (!user_cmd || !cheat::local_player || !cheat::local_player->is_alive() || user_cmd->command_number == 0)
        return create_move_original(ecx, edx, frame_time, user_cmd);

    if (create_move_original(ecx, edx, frame_time, user_cmd))
        interfaces::prediction->set_local_view_angles(user_cmd->view_angles);

    cheat::user_cmd = user_cmd;
    cheat::original_angles = user_cmd->view_angles;

    if (interfaces::client_state == nullptr)
        return create_move_original(ecx, edx, frame_time, user_cmd);

    uintptr_t *frame_pointer;
    __asm mov frame_pointer, ebp;
    bool &send_packet = *reinterpret_cast<bool *>(*frame_pointer - 0x1C);

    lua::callbacks::setup_command(user_cmd, send_packet);

    features::fake_ping::on_create_move();

    if (cheat::local_player->is_alive()) {
        features::legitbot::sample_angle_data(user_cmd->view_angles);

        features::engine_prediction::store();

        if (settings.miscellaneous.movement.edge_bug_assist_hotkey)
            features::engine_prediction::create_edgebug_entry(user_cmd);

        // if its called before pre pred it breaks.
        features::movement::autostrafer(user_cmd);

        features::movement::pre_prediction(user_cmd);
        const auto pre_flags = cheat::local_player->get_flags();

        features::engine_prediction::start_prediction();

        c_weapon *local_weapon = (c_weapon *) cheat::local_player->get_active_weapon_handle().get();
        if (local_weapon) {
            if (features::legitbot::update_settings(local_weapon)) {
                features::legitbot::lag_comp::on_create_move(user_cmd);
                features::legitbot::on_create_move(user_cmd, local_weapon);
                features::legitbot::triggerbot::on_create_move(user_cmd, local_weapon);
            }

            features::nade_prediction::on_create_move(user_cmd, local_weapon);
            features::miscellaneous::on_create_move(user_cmd, local_weapon);
        }

        features::engine_prediction::end_prediction();

        const auto post_flags = cheat::local_player->get_flags();
        features::movement::strafe_optimizer(user_cmd, pre_flags, post_flags);
        features::movement::post_prediction(user_cmd, pre_flags, post_flags);

        util::movement_fix(user_cmd);
        features::movement::slide_walk(user_cmd);
    }

    features::miscellaneous::rank_reveal(user_cmd);
    features::miscellaneous::preserve_killfeed();

    features::movement::blockbot(user_cmd);

    lua::callbacks::run_command(user_cmd);

    math::normalize_angles(user_cmd->view_angles);
    math::clamp_angles(user_cmd->view_angles);

    return false;
}
