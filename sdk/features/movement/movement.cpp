#include "movement.h"
#include "engine prediction/engine_prediction.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/patterns/patterns.h"
#include "../../core/settings/settings.h"

#include "../../engine/hash/hash.h"
#include "../../engine/input/input.h"
#include "../../engine/logging/logging.h"
#include "../../engine/math/math.h"
#include "../../engine/security/xorstr.h"

void features::movement::pre_prediction(c_user_cmd *user_cmd) {

    if (settings.miscellaneous.movement.no_duck_cooldown) {
        user_cmd->buttons |= BUTTON_IN_BULLRUSH;
    }

    if (settings.miscellaneous.movement.bunny_hop) {
        if (cheat::local_player->get_move_type() == MOVE_TYPE_LADDER || input::is_key_down(settings.miscellaneous.movement.jump_bug_hotkey))
            return;

        if (!(cheat::local_player->get_flags() & ENTITY_FLAG_ONGROUND))
            user_cmd->buttons &= ~BUTTON_IN_JUMP;
    }

    if (settings.miscellaneous.movement.air_duck) {
        if (!(cheat::local_player->get_flags() & ENTITY_FLAG_ONGROUND))
            user_cmd->buttons |= BUTTON_IN_DUCK;
    }

    // putting these here because the code
    // is quite ugly and not really appealing
    fast_walk(user_cmd);
    slide_walk(user_cmd);
}

void features::movement::post_prediction(c_user_cmd *user_cmd, int pre_flags, int post_flags) {

    if (settings.miscellaneous.movement.jump_bug && input::is_key_down(settings.miscellaneous.movement.jump_bug_hotkey)) {
        cheat::b_predicting = true;

        if (!(pre_flags & ENTITY_FLAG_ONGROUND) && post_flags & ENTITY_FLAG_ONGROUND) {
            user_cmd->buttons |= BUTTON_IN_DUCK;
        }

        if (post_flags & ENTITY_FLAG_ONGROUND) {
            user_cmd->buttons &= ~BUTTON_IN_JUMP;
        }
    }

    if (settings.miscellaneous.movement.edge_bug && input::is_key_down(settings.miscellaneous.movement.edge_bug_hotkey)) {
        if (!(pre_flags & ENTITY_FLAG_ONGROUND) && post_flags & ENTITY_FLAG_ONGROUND)
            user_cmd->buttons |= BUTTON_IN_DUCK;
    }

    if (settings.miscellaneous.movement.edge_jump && input::is_key_down(settings.miscellaneous.movement.edge_jump_hotkey)) {
        if (pre_flags & ENTITY_FLAG_ONGROUND && !(post_flags & ENTITY_FLAG_ONGROUND))
            user_cmd->buttons |= BUTTON_IN_JUMP;
    }
}

void features::movement::predict_edgebug(c_user_cmd *user_cmd) {
    // credits: clarity.tk

    vector_t unpredicted_velocity = cheat::local_player->get_velocity();

    if ((int) roundf(cheat::local_player->get_velocity().z) == 0.0 || (cheat::unpredicted_flags & ENTITY_FLAG_ONGROUND) != 0) {
        features::movement::edgebug_container::predicted_successful = 0;
        features::movement::edgebug_container::prediction_failed = 1;
    }
    else if (unpredicted_velocity.z < -6.0 && cheat::local_player->get_velocity().z > unpredicted_velocity.z && cheat::local_player->get_velocity().z < -6.0 &&
             (cheat::local_player->get_flags() & 1) == 0 && cheat::local_player->get_move_type() != MOVE_TYPE_NOCLIP && cheat::local_player->get_move_type() != MOVE_TYPE_LADDER) {
        const auto previous_velocity = cheat::local_player->get_velocity().z;

        engine_prediction::run_command(user_cmd);

        static auto sv_gravity = interfaces::convar_system->find_convar(XORSTR("sv_gravity"));
        const float gravity_velocity_constant = roundf((-sv_gravity->get_float()) * interfaces::global_vars->interval_per_tick + previous_velocity);

        if (gravity_velocity_constant == roundf(cheat::local_player->get_velocity().z)) {
            features::movement::edgebug_container::predicted_successful = 1;
            features::movement::edgebug_container::prediction_failed = 0;
        }
        else {
            features::movement::edgebug_container::predicted_successful = 0;
            features::movement::edgebug_container::prediction_failed = 1;
        }
    }
}

void features::movement::edgebug_assist(c_user_cmd *user_cmd) {

    vector_t unpredicted_velocity = cheat::local_player->get_velocity();

    if (!input::is_key_down(settings.miscellaneous.movement.edge_bug_assist_hotkey))
        return;

    if (!features::movement::edgebug_container::predicted_successful) {

        features::movement::edgebug_container::should_duck = 0;

        for (auto prediction_ticks_ran = 0; prediction_ticks_ran < 2; ++prediction_ticks_ran) {

            if (prediction_ticks_ran == 1) {
                features::movement::edgebug_container::should_duck = 1;
                engine_prediction::apply_edgebug_data(user_cmd);
            }

            if (!prediction_ticks_ran)
                engine_prediction::apply_edgebug_flags();

            for (auto radius_checked = 1; radius_checked <= settings.miscellaneous.movement.edge_bug_radius; ++radius_checked) {

                if (prediction_ticks_ran == 1) {
                    engine_prediction::run_command(user_cmd);
                }

                if ((cheat::unpredicted_flags & 1) != 0 || unpredicted_velocity.z > 0.0) {
                    features::movement::edgebug_container::predicted_successful = 0;
                    break;
                }

                predict_edgebug(user_cmd);

                if (features::movement::edgebug_container::predicted_successful) {
                    features::movement::edgebug_container::prediction_ticks = radius_checked;
                    features::movement::edgebug_container::prediction_timestamp = interfaces::global_vars->tick_count;
                    features::movement::edgebug_container::mouse_offset = abs(user_cmd->mouse_dx);
                    break;
                }

                if (features::movement::edgebug_container::prediction_failed) {
                    features::movement::edgebug_container::prediction_failed = 0;
                    break;
                }

                if (!prediction_ticks_ran) {
                    engine_prediction::run_command(user_cmd);
                }
            }

            if (features::movement::edgebug_container::predicted_successful)
                break;
        }
    }

    if (features::movement::edgebug_container::predicted_successful) {

        if (interfaces::global_vars->tick_count < features::movement::edgebug_container::prediction_ticks + features::movement::edgebug_container::prediction_timestamp) {
            user_cmd->forward_move = 0.0;
            user_cmd->side_move = 0.0;

            if (features::movement::edgebug_container::should_duck)
                user_cmd->buttons |= BUTTON_IN_DUCK;
        }

        else {
            features::movement::edgebug_container::predicted_successful = 0;
            features::movement::edgebug_container::should_duck = 0;
        }
    }
}

void features::movement::fast_walk(c_user_cmd *user_cmd) {

    if (!settings.miscellaneous.movement.fast_walk)
        return;

    if (!input::is_key_down(settings.miscellaneous.movement.fast_walk_hotkey))
        return;

    if (user_cmd->buttons & BUTTON_IN_SPEED)
        user_cmd->buttons &= ~BUTTON_IN_SPEED;

    const vector_t vel = cheat::local_player->get_velocity();
    const float speed = vel.length_2d();

    if (speed < 126.0f)
        return;

    static auto sv_accelerate = interfaces::convar_system->find_convar(XORSTR("sv_accelerate"));

    constexpr float surf_friction = 1.0f;

    const float accel = sv_accelerate->get_float();
    const float max_accel_speed = accel * interfaces::global_vars->interval_per_tick * speed * surf_friction;

    float wish_speed;

    if (speed - max_accel_speed <= -1.0f) {
        wish_speed = max_accel_speed / (speed / (accel * interfaces::global_vars->interval_per_tick));
    }
    else {
        wish_speed = max_accel_speed;
    }

    vector_t inv_dir = math::vector_to_angles(vel * -1.0f);
    vector_t fix_dumb_error;

    interfaces::engine_client->get_view_angles(fix_dumb_error);

    inv_dir.y = fix_dumb_error.y - inv_dir.y;
    inv_dir = math::angle_to_vector(inv_dir);

    user_cmd->forward_move = inv_dir.x * wish_speed;
    user_cmd->side_move = inv_dir.y * wish_speed;
}

void features::movement::slide_walk(c_user_cmd *user_cmd) {
    user_cmd->buttons &= ~BUTTON_IN_MOVE_RIGHT;
    user_cmd->buttons &= ~BUTTON_IN_MOVE_LEFT;
    user_cmd->buttons &= ~BUTTON_IN_LEFT;
    user_cmd->buttons &= ~BUTTON_IN_RIGHT;
    user_cmd->buttons &= ~BUTTON_IN_FORWARD;
    user_cmd->buttons &= ~BUTTON_IN_BACK;

    const auto move_type = cheat::local_player->get_move_type();

    if (settings.miscellaneous.movement.slide_walk && move_type != MOVE_TYPE_NOCLIP && move_type != MOVE_TYPE_LADDER) {
        if (user_cmd->forward_move < 0.0f) {
            user_cmd->buttons |= BUTTON_IN_FORWARD;
        }
        else if (user_cmd->forward_move > 0.0f) {
            user_cmd->buttons |= BUTTON_IN_BACK;
        }

        if (user_cmd->side_move < 0.0f) {
            user_cmd->buttons |= BUTTON_IN_MOVE_RIGHT;
            user_cmd->buttons |= BUTTON_IN_RIGHT;
        }
        else if (user_cmd->side_move > 0.0f) {
            user_cmd->buttons |= BUTTON_IN_MOVE_LEFT;
            user_cmd->buttons |= BUTTON_IN_LEFT;
        }
    }
    else {
        if (user_cmd->forward_move > 0.0f) {
            user_cmd->buttons |= BUTTON_IN_FORWARD;
        }
        else if (user_cmd->forward_move < 0.0f) {
            user_cmd->buttons |= BUTTON_IN_BACK;
        }

        if (user_cmd->side_move > 0.0f) {
            user_cmd->buttons |= BUTTON_IN_MOVE_RIGHT;
            user_cmd->buttons |= BUTTON_IN_RIGHT;
        }
        else if (user_cmd->side_move < 0.0f) {
            user_cmd->buttons |= BUTTON_IN_MOVE_LEFT;
            user_cmd->buttons |= BUTTON_IN_LEFT;
        }
    }
}
