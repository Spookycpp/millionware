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

    if (!cheat::local_player->is_alive())
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
    else if ((curVel.z < 1.f && unpredicted_velocity.z + 0.5 < curVel.z && next_possible_eb < interfaces::global_vars->current_time)) {
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
