#include "engine_prediction.h"
#include "../movement.h"

#include "../../../core/cheat/cheat.h"

#include "../../../source engine/client_prediction.h"

#include "../../../core/settings/settings.h"
#include "../../../core/util/util.h"
#include "../../../engine/input/input.h"
#include "../../../engine/math/math.h"
#include "../../../engine/security/xorstr.h"
#include <array>

namespace features::engine_prediction {

    void start_prediction() {

        // fix tickbase if game didnt render previous tick
        if (last_command) {
            if (last_command->has_been_predicted) {
                correct_tickbase = cheat::local_player->get_tick_base();
            } else {
                ++correct_tickbase;
            }
        }

        last_command = cheat::user_cmd;
        old_tickbase = cheat::local_player->get_tick_base();

        old_curtime = interfaces::global_vars->current_time;
        old_frametime = interfaces::global_vars->frame_time;

        cheat::local_player->get_tick_base() = correct_tickbase;
        interfaces::global_vars->current_time = float(cheat::local_player->get_tick_base()) * interfaces::global_vars->interval_per_tick;
        interfaces::global_vars->frame_time = interfaces::global_vars->interval_per_tick;

        interfaces::game_movement->start_track_prediction_errors(cheat::local_player);

        movedata_t data;
        memset(&data, 0, sizeof(movedata_t));

        interfaces::move_helper->set_host(cheat::local_player);
        interfaces::prediction->setup_move(cheat::local_player, cheat::user_cmd, interfaces::move_helper, &data);

        if (input::is_key_down(settings.miscellaneous.movement.edge_bug_assist_hotkey)) {
            if (features::movement::should_duck)
                data.m_nButtons |= 4u;
            data.m_nButtons &= ~0x200u;
            data.m_flForwardMove = 0;
            data.m_flSideMove = 0;
            data.m_nButtons &= ~0x400u;
            data.m_nButtons &= ~8u;
            data.m_nButtons &= ~0x10u;
        }

        interfaces::game_movement->process_movement(cheat::local_player, &data);
        interfaces::prediction->finish_move(cheat::local_player, cheat::user_cmd, &data);
    }

    void repredict() {
        /*if (interfaces::client_state->delta_tick > 0) {
            interfaces::prediction->update(interfaces::client_state->delta_tick, interfaces::client_state->delta_tick > 0,
        interfaces::client_state->last_command_ack, interfaces::client_state->last_command + interfaces::client_state->choked_commands);
        }*/
    }

    void engine_prediction::store() {
        cheat::unpredicted_flags = cheat::local_player->get_flags();
        cheat::unpredicted_curtime = interfaces::global_vars->current_time;
        cheat::unpredicted_velocity = cheat::local_player->get_velocity();
    }

    void create_edgebug_entry(c_user_cmd *user_cmd) {
        features::movement::fall_velocity = cheat::local_player->get_fall_velocity();
        features::movement::flags = cheat::local_player->get_flags();
        features::movement::origin = cheat::local_player->get_vec_origin();
        features::movement::absolute_origin = cheat::local_player->get_abs_origin();
        features::movement::ground_ent = cheat::local_player->get_ground_entity();
        interfaces::prediction->setup_move(cheat::local_player, user_cmd, interfaces::move_helper, &features::movement::movement_data);
    }

    void apply_edgebug_data(c_user_cmd *user_cmd) {
        interfaces::prediction->finish_move(cheat::local_player, interfaces::move_helper, &features::movement::movement_data);
        features::movement::flags |= 2;
        features::movement::movement_data.m_nButtons |= 4;
        cheat::local_player->get_flags() = features::movement::flags;
        cheat::local_player->get_fall_velocity() = features::movement::fall_velocity;
        cheat::local_player->set_absolute_origin(features::movement::absolute_origin);
        cheat::local_player->get_vec_origin() = features::movement::origin;
        cheat::local_player->get_ground_entity() = features::movement::ground_ent;
    }

    void apply_edgebug_flags() {

        cheat::local_player->get_flags() = features::movement::flags;
    }

    void end_prediction() {
        interfaces::game_movement->finish_track_prediction_errors(cheat::local_player);
        interfaces::move_helper->set_host(nullptr);

        cheat::local_player->get_tick_base() = old_tickbase;

        interfaces::global_vars->current_time = engine_prediction::old_curtime;
        interfaces::global_vars->frame_time = engine_prediction::old_frametime;
    }

    void run_in_prediction(std::function<void()> fn) {

        if (cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
            return;

        start_prediction();

        fn();

        end_prediction();
    }

} // namespace features::engine_prediction