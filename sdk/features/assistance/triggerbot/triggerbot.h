#pragma once

#include "../../../source engine/entity.h"
#include "../../../source engine/input.h"
#include "../../../source engine/vector.h"

namespace features::legitbot::triggerbot {
	void on_create_move(c_user_cmd* cmd, c_weapon* wpn);

	void think(c_user_cmd* cmd, c_weapon* wpn);

	bool trace_to_target(c_weapon* wpn, const vector_t& start_pos, const vector_t& end_pos);
	bool trace_to_backtracked_target(c_user_cmd* cmd, c_weapon* wpn, const vector_t& start_pos, const vector_t& end_pos);

	bool is_valid_target(c_entity* target);

	bool should_activate();
}