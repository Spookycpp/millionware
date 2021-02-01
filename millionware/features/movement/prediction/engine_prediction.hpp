#pragma once

#include "../../../core/interfaces.hpp"
#include "../../../sdk/user_cmd.hpp"

#include <functional>

namespace engine_prediction {
	inline float old_curtime, old_frametime;
	inline int old_tickbase;
	inline bool old_in_prediction, old_is_first_time_predicted;

	inline int correct_tickbase;
	inline user_cmd_t* last_command;

	void init();
	void start_prediction(user_cmd_t* cmd);
	void store(user_cmd_t* cmd);
	void end_prediction(user_cmd_t* cmd);

	void run_in_prediction(user_cmd_t* cmd, std::function< void() > fn);
}