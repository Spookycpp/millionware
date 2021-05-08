#include <mutex>

#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../engine/math/math.h"
#include "../features/assistance/lagcompensation/lag_comp.h"
#include "../features/assistance/legitbot.h"
#include "../features/assistance/triggerbot/triggerbot.h"
#include "../features/fake ping/fake_ping.h"
#include "../features/miscellaneous/miscellaneous.h"
#include "../features/movement/engine prediction/engine_prediction.h"
#include "../features/movement/movement.h"
#include "../features/visuals/world/world.h"

bool __fastcall hooks::create_move(c_client_mode *ecx, uintptr_t edx, float frame_time, c_user_cmd *user_cmd) {

    const auto result = create_move_original(ecx, edx, frame_time, user_cmd);

    if (!user_cmd || !user_cmd->command_number || !user_cmd->tick_count)
        return result;

    if (result)
        interfaces::engine_client->set_view_angles(user_cmd->view_angles);

    features::fake_ping::on_create_move();

    if (cheat::local_player->is_alive()) {
        features::legitbot::sample_angle_data(user_cmd->view_angles);

        features::movement::pre_prediction(user_cmd);
        const auto pre_flags = cheat::local_player->get_flags();
        features::engine_prediction::on_create_move(user_cmd);
        const auto post_flags = features::engine_prediction::get_predicted_flags();
        features::movement::post_prediction(user_cmd, pre_flags, post_flags);

        features::movement::edgebug_assist(user_cmd);

        c_weapon *local_weapon = (c_weapon *) cheat::local_player->get_active_weapon_handle().get();

        if (local_weapon) {
            if (features::legitbot::update_settings(local_weapon)) {
                features::legitbot::lag_comp::on_create_move(user_cmd);
                features::legitbot::on_create_move(user_cmd, local_weapon);
                features::legitbot::triggerbot::on_create_move(user_cmd, local_weapon);
            }

            features::miscellaneous::on_create_move(user_cmd, local_weapon);
        }

        features::movement::slide_walk(user_cmd);
    }

    features::miscellaneous::rank_reveal(user_cmd);

    math::normalize_angles(user_cmd->view_angles);
    math::clamp_angles(user_cmd->view_angles);

    return false;
}
