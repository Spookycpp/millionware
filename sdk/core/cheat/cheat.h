#pragma once

#include "../../source engine/entity.h"

namespace cheat
{
	bool init();
	bool undo();

	inline c_player *local_player;

	inline uint32_t unpredicted_flags;
	inline bool b_predicting;
	inline bool landed = false;
	inline bool stop_movement;
}
