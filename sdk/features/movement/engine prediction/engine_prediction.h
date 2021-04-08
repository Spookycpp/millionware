#pragma once

#include "../../../core/interfaces/interfaces.h"
#include "../../../source engine/input.h"

#include <functional>

namespace engine_prediction {
	inline float old_curtime, old_frametime;
	inline int old_tickbase;
	inline bool old_in_prediction, old_is_first_time_predicted;

	inline int correct_tickbase;
	inline c_user_cmd* last_command;

	void init();
	void start_prediction(c_user_cmd* user_cmd);
	void store(c_user_cmd* user_cmd);
	void end_prediction(c_user_cmd* user_cmd);
}