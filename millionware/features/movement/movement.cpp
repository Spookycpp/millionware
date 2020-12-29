#include "movement.hpp"

#include "../../core/cheat.hpp"
#include "../../core/config.hpp"
#include "../../core/interfaces.hpp"
#include "../../core/patterns.hpp"

#include "../../utils/xorstr.hpp"
#include "../../utils/hash.hpp"

void features::movement::bunny_hop(user_cmd_t* user_cmd) {
	if (!config::get<bool>(FNV_CT("misc.movement.bunny_hop")))
		return;

	if (user_cmd->buttons & BUTTON_IN_JUMP && !(cheat::local_player->flags() & ENTITY_FLAG_ONGROUND))
		user_cmd->buttons &= ~BUTTON_IN_JUMP;
}

void features::movement::infinite_duck(user_cmd_t* user_cmd) {
	if (!config::get<bool>(FNV_CT("misc.movement.infinite_duck")))
		return;

	user_cmd->buttons |= BUTTON_IN_BULLRUSH;
}
