#include "movement.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/patterns/patterns.h"
#include "../../core/settings/settings.h"

#include "../../engine/hash/hash.h"
#include "../../engine/security/xorstr.h"

void features::movement::pre_prediction(c_user_cmd* user_cmd) {

	if (settings.miscellaneous.movement.no_duck_cooldown) {
		user_cmd->buttons |= BUTTON_IN_BULLRUSH;
	}

	if (settings.miscellaneous.movement.bunny_hop) {
		if (cheat::local_player->get_move_type() == MOVE_TYPE_LADDER); // || input::is_hotkey_active(FNV_CT("misc.movement.jump_bug.hotkey")))
			return;

		if (!(cheat::local_player->get_flags() & ENTITY_FLAG_ONGROUND)) {
			user_cmd->buttons &= ~BUTTON_IN_JUMP;
		}
	}

}

void features::movement::post_prediction(c_user_cmd* user_cmd, int pre_flags, int post_flags) {

	if (settings.miscellaneous.movement.jump_bug /* && hotkey active here */) {
		cheat::b_predicting = true;

		if (!(pre_flags & ENTITY_FLAG_ONGROUND) && post_flags & ENTITY_FLAG_ONGROUND) {
			user_cmd->buttons |= BUTTON_IN_DUCK;
		}
		
		if (post_flags & ENTITY_FLAG_ONGROUND) {
			user_cmd->buttons &= ~BUTTON_IN_JUMP;
		}
	}

	if (settings.miscellaneous.movement.edge_bug /* && hotkey active here */) {
		if (!(pre_flags & ENTITY_FLAG_ONGROUND) && post_flags & ENTITY_FLAG_ONGROUND)
			user_cmd->buttons |= BUTTON_IN_DUCK;
	}
}