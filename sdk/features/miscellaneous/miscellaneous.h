#pragma once

#include "../../source engine/client_dll.h"
#include "../../source engine/entity.h"
#include "../../source engine/input.h"
#include "../../source engine/weapon_info.h"
#include "../../source engine/view_setup.h"

namespace features::miscellaneous {

	void auto_pistol(c_user_cmd* user_cmd);
	void auto_accept();
	void rank_reveal(c_user_cmd* user_cmd);

	void override_fov(view_setup_t* view_setup);

	void post_processing();
	void panorama_blur();
	void force_crosshair();
	void flash_alpha();
	void recoil_crosshair();
	void ragdoll_push();
	void ragdoll_float();
	void skybox_changer();
}