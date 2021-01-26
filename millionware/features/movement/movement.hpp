#pragma once
#include "../../sdk/user_cmd.hpp"

namespace features::movement {
	void pre_prediction(user_cmd_t* user_cmd);
	void post_prediction(user_cmd_t* user_cmd, int pre_flags, int post_flags);
}