#pragma once

#include "../../source engine/client_dll.h"
#include "../../source engine/entity.h"
#include "../../source engine/input.h"
#include "../../source engine/weapon_info.h"

namespace features::movement {
	inline vector_t old_vel   = 0.f;
	inline vector_t unpredicted_velocity = { 0.f, 0.f, 0.f };
	inline bool	stop_movement = false;
	inline bool is_edge_bugging;
	inline float next_possible_eb = 0.f;

	void pre_prediction(c_user_cmd* user_cmd);
	void post_prediction(c_user_cmd* user_cmd, int pre_flags, int post_flags);

	void edgebug_assist(c_user_cmd* user_cmd);
}