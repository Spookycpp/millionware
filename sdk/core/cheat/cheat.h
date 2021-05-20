#pragma once

#include "../../source engine/entity.h"
#include "../../source engine/input.h"

namespace cheat
{
	bool init();
	bool undo();

	inline c_player *local_player;
	inline c_user_cmd *user_cmd;

	inline uint32_t unpredicted_flags;
    inline vector_t unpredicted_velocity;
    inline e_move_type unpredicted_movetype;
    inline float unpredicted_curtime;

	inline bool b_predicting;
	inline bool landed = false;
	inline bool stop_movement;
	inline uintptr_t run_command;
    inline int tick_base_shift;
}
