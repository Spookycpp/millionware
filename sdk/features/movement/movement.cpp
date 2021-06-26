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

        cheat::b_predicting = false;
    }

    if (settings.miscellaneous.movement.edge_bug && input::is_key_down(settings.miscellaneous.movement.edge_bug_hotkey)) {
        if (!(pre_flags & ENTITY_FLAG_ONGROUND) && post_flags & ENTITY_FLAG_ONGROUND)
            user_cmd->buttons |= BUTTON_IN_DUCK;
    }

    if (settings.miscellaneous.movement.edge_jump && input::is_key_down(settings.miscellaneous.movement.edge_jump_hotkey)) {
        if (pre_flags & ENTITY_FLAG_ONGROUND && !(post_flags & ENTITY_FLAG_ONGROUND))
            user_cmd->buttons |= BUTTON_IN_JUMP;
    }

    edgebug_assist(user_cmd);
}

void features::movement::predict_edgebug(c_user_cmd *user_cmd) {
    cheat::b_predicting = true;

    if ((int) roundf(cheat::local_player->get_velocity().z) == 0.0 || (cheat::unpredicted_flags & ENTITY_FLAG_ONGROUND) != 0) {
        features::movement::predicted_successful = 0;
        features::movement::prediction_failed = 1;
    }
    else if (cheat::unpredicted_velocity.z < -6.0 && cheat::local_player->get_velocity().z > cheat::unpredicted_velocity.z &&
             cheat::local_player->get_velocity().z < -6.0 && (cheat::local_player->get_flags() & 1) == 0 &&
             cheat::local_player->get_move_type() != MOVE_TYPE_NOCLIP && cheat::local_player->get_move_type() != MOVE_TYPE_LADDER) {
        const auto previous_velocity = cheat::local_player->get_velocity().z;

        engine_prediction::start_prediction();
        engine_prediction::end_prediction();

        static auto sv_gravity = interfaces::convar_system->find_convar(XORSTR("sv_gravity"));
        const float gravity_velocity_constant = roundf((-sv_gravity->get_float()) * interfaces::global_vars->interval_per_tick + previous_velocity);

        if (gravity_velocity_constant == roundf(cheat::local_player->get_velocity().z)) {
            features::movement::predicted_successful = 1;
            features::movement::prediction_failed = 0;
        }
        else {
            features::movement::predicted_successful = 0;
            features::movement::prediction_failed = 1;
        }
    }
}

void features::movement::edgebug_assist(c_user_cmd *user_cmd) {

    if (!input::is_key_down(settings.miscellaneous.movement.edge_bug_assist_hotkey))
        return;

    if (!features::movement::predicted_successful) {

        cheat::b_predicting = true;

        features::movement::should_duck = 0;

        for (auto prediction_ticks_ran = 0; prediction_ticks_ran < 2; ++prediction_ticks_ran) {

            if (prediction_ticks_ran == 1) {
                features::movement::should_duck = 1;
                engine_prediction::apply_edgebug_data(user_cmd);
            }

            if (!prediction_ticks_ran)
                engine_prediction::apply_edgebug_flags();

            for (auto radius_checked = 1; radius_checked <= settings.miscellaneous.movement.edge_bug_radius; ++radius_checked) {

                if (prediction_ticks_ran == 1) {
                    engine_prediction::start_prediction();
                    engine_prediction::end_prediction();
                }

                if ((cheat::unpredicted_flags & 1) != 0 || cheat::unpredicted_velocity.z > 0.0) {
                    features::movement::predicted_successful = 0;
                    break;
                }

                predict_edgebug(user_cmd);

                if (features::movement::predicted_successful) {
                    features::movement::prediction_ticks = radius_checked;
                    features::movement::prediction_timestamp = interfaces::global_vars->tick_count;
                    features::movement::mouse_offset = abs(user_cmd->mouse_dx);
                    break;
                }

                if (features::movement::prediction_failed) {
                    features::movement::prediction_failed = 0;
                    break;
                }

                if (!prediction_ticks_ran) {
                    engine_prediction::start_prediction();
                    engine_prediction::end_prediction();
                }

            }

            if (features::movement::predicted_successful)
                break;
        }
    }

    if (features::movement::predicted_successful) {

        if (interfaces::global_vars->tick_count <
            features::movement::prediction_ticks + features::movement::prediction_timestamp) {
            user_cmd->forward_move = 0.0;
            user_cmd->side_move = 0.0;

            if (features::movement::should_duck)
                user_cmd->buttons |= BUTTON_IN_DUCK;
        }
        else {
            features::movement::predicted_successful = 0;
            features::movement::should_duck = 0;
        }
    }

    cheat::b_predicting = false;
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

void features::movement::strafe_optimizer(c_user_cmd *user_cmd) {

}
