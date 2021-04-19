#pragma once

#include "../../source engine/client_dll.h"
#include "../../source engine/entity.h"
#include "../../source engine/input.h"
#include "../../source engine/weapon_info.h"
#include "../../source engine/view_setup.h"

namespace features::miscellaneous {

	void on_create_move(c_user_cmd* user_cmd, c_weapon* weapon);
	void on_frame_stage_notify(e_client_frame_stage frame_stage);
	void on_override_view(view_setup_t* view_setup);

	// usercmd
	void auto_pistol(c_user_cmd* user_cmd);
	void auto_accept();
	void rank_reveal(c_user_cmd* user_cmd);

	// override view
	void override_fov(view_setup_t* view_setup);

	// visual
	void post_processing();
	void panorama_blur();
	void force_crosshair();
	void flash_alpha();
	void recoil_crosshair();
	void ragdoll_push();
	void ragdoll_float();
	void skybox_changer(int skybox);
	void foot_fx();
	void foot_trail();
	void viewmodel_offset();
}