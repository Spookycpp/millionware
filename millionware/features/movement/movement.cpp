#include "movement.hpp"

#include "../../core/cheat.hpp"
#include "../../core/config.hpp"
#include "../../core/interfaces.hpp"
#include "../../core/patterns.hpp"

#include "../../utils/hash/hash.hpp"
#include "../../utils/xorstr/xorstr.hpp"
#include "../../utils/input/input.hpp"

void features::movement::pre_prediction(user_cmd_t* user_cmd) {

	if (config::get<bool>(FNV_CT("misc.movement.infinite_duck"))) {
		user_cmd->buttons |= BUTTON_IN_BULLRUSH;
	}

	if (config::get<bool>(FNV_CT("misc.movement.bunny_hop")))
	{
		if (cheat::local_player->move_type() == MOVE_TYPE_LADDER || input::is_key_down(VK_XBUTTON2))
			return;

		if (!(cheat::local_player->flags() & ENTITY_FLAG_ONGROUND))
			user_cmd->buttons &= ~BUTTON_IN_JUMP;
	}
}

void features::movement::post_prediction(user_cmd_t* user_cmd, int pre_flags, int post_flags) {

	if (config::get<bool>(FNV_CT("misc.movement.jump_bug")) && input::is_key_down(VK_XBUTTON2)) {
		cheat::b_predicting = true;

		if (!(pre_flags & ENTITY_FLAG_ONGROUND) && post_flags & ENTITY_FLAG_ONGROUND) {
			user_cmd->buttons |= BUTTON_IN_DUCK;
		}

		if (post_flags & ENTITY_FLAG_ONGROUND) {
			user_cmd->buttons &= ~BUTTON_IN_JUMP;
		}
	}

	if (config::get<bool>(FNV_CT("misc.movement.edge_bug")) && input::is_key_down('C')) {
		if (!(pre_flags & ENTITY_FLAG_ONGROUND) && post_flags & ENTITY_FLAG_ONGROUND)
			user_cmd->buttons |= BUTTON_IN_DUCK;
	}

	if (config::get<bool>(FNV_CT("misc.movement.edge_jump")) && input::is_key_down('E')) {
		if (pre_flags & ENTITY_FLAG_ONGROUND && !(post_flags & ENTITY_FLAG_ONGROUND))
			user_cmd->buttons |= BUTTON_IN_JUMP;
	}

}
