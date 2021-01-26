#pragma once

#include "../../sdk/user_cmd.hpp"
#include "../../sdk/weapon_info.hpp"
#include "../../sdk/entity.hpp"
#include "../../sdk/client_dll.hpp"

namespace features::miscellaneous {
	void auto_accept();
	void auto_pistol(user_cmd_t* user_cmd);
	void rank_reveal();

	void override_fov(view_setup_t* view_setup);
	void panorama_blur();
	void post_processing();
	void force_crosshair();
	void ragdoll_push();
	void ragdoll_float();
}