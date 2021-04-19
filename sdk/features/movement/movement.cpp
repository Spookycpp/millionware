#include "movement.h"
#include "engine prediction/engine_prediction.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/patterns/patterns.h"
#include "../../core/settings/settings.h"

#include "../../engine/hash/hash.h"
#include "../../engine/input/input.h"
#include "../../engine/security/xorstr.h"
#include "../../engine/logging/logging.h"
#include "../../engine/math/math.h"

void features::movement::pre_prediction(c_user_cmd* user_cmd) {

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

void features::movement::post_prediction(c_user_cmd* user_cmd, int pre_flags, int post_flags) {

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

void features::movement::edgebug_assist(c_user_cmd* user_cmd) { // aiden
    // we are going to try and do this without pasting

    c_user_cmd* backupCmd = user_cmd;
    static auto backupLocal = cheat::local_player;
    bool shouldDuck = false;

    if (!input::is_key_down(settings.miscellaneous.movement.edge_bug_assist_hotkey)) 
        return;

    if (cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
        return;

    if (cheat::local_player->get_velocity().z > 0)
        return;

    vector_t vel;

    for (int i = 0; i <= settings.miscellaneous.movement.edge_bug_radius; i++) {
        engine_prediction::start_prediction(user_cmd);
        cheat::b_predicting = true;
        backupCmd = user_cmd;
        backupLocal = cheat::local_player;

        vel = backupLocal->get_velocity();

        if (!(backupLocal->get_flags() & ENTITY_FLAG_ONGROUND) && backupLocal->get_move_type() != MOVE_TYPE_NOCLIP) {
            if (vel.z < 1.f && old_vel.z + 0.5 < vel.z) {
                if (i != 0) {
                    if (i < 7)
                        shouldDuck = true;
                    cheat::stop_movement = true;
                    logging::info("predicted edgebug on tick");
                }
                break;
            }
        }
        old_vel = vel;
    }

    engine_prediction::end_prediction(user_cmd);
    cheat::b_predicting = false;

    if (cheat::stop_movement) {
        if (settings.miscellaneous.movement.edge_bug_movement) {
        user_cmd->side_move = 0;
        user_cmd->forward_move = 0;
        }
    }

    vector_t curVel = cheat::local_player->get_velocity();

    // wow this if statement is so fucking ugly

    if (cheat::local_player->get_flags() & ENTITY_FLAG_ONGROUND) {
        cheat::stop_movement = false;
        shouldDuck = false;
    }
    if (std::floor(old_vel.z) < -7 && (cheat::local_player->get_velocity().z / old_vel.z) <= 0.7) {
        if (shouldDuck && settings.miscellaneous.movement.edge_bug_crouch) {
            cheat::stop_movement = false;
            shouldDuck = false;
            user_cmd->buttons |= BUTTON_IN_DUCK;
            logging::info("pog");
        }
    }

    next_possible_eb = interfaces::global_vars->current_time;
    unpredicted_velocity = curVel;
}

void features::movement::fast_walk(c_user_cmd* user_cmd) {

    if (!settings.miscellaneous.movement.fast_walk) 
        return;    

    if (!input::is_key_down(settings.miscellaneous.movement.fast_walk_hotkey))
        return;    

    if (user_cmd->buttons & BUTTON_IN_SPEED) 
        user_cmd->buttons &= ~BUTTON_IN_SPEED;
    
    const vector_t vel = cheat::local_player->get_velocity();
    const float  speed = vel.length_2d();

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

void features::movement::slide_walk(c_user_cmd* user_cmd) {
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
