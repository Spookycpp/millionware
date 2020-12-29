#pragma once
#include "../../../sdk/user_cmd.hpp"
#include "../../../sdk/vector.hpp"
#include "../../../sdk/entity.hpp"

namespace features::aimbot::triggerbot {
	void on_create_move(user_cmd_t* cmd, c_weapon* wpn);

	void think(user_cmd_t* cmd, c_weapon* wpn);

	bool trace_to_target(C_BaseCombatWeapon* wpn, const vector3_t& start_pos, const vector3_t& end_pos);
	bool trace_to_backtracked_target(user_cmd_t* cmd, c_weapon* wpn, const vector3_t& start_pos, const vector3_t& end_pos);

	bool is_valid_target(IClientEntity* target);

	bool should_activate();
}