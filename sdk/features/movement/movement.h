#pragma once

#include "../../source engine/client_dll.h"
#include "../../source engine/entity.h"
#include "../../source engine/input.h"
#include "../../source engine/weapon_info.h"

namespace features::movement {
	void pre_prediction(c_user_cmd* user_cmd);
	void post_prediction(c_user_cmd* user_cmd, int pre_flags, int post_flags);
}