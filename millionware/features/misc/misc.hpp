#pragma once

#include "../../sdk/user_cmd.hpp"

namespace features::misc {

	void bunny_hop(user_cmd_t* user_cmd);
	void infinite_duck(user_cmd_t* user_cmd);
	void auto_strafer(user_cmd_t* user_cmd);
	void auto_pistol(user_cmd_t* user_cmd);

	void auto_accept();
	void rank_reveal();
	void clantag();

	void name_spam();
	void remove_panorama_blur();
	void remove_post_processing();
	void flash_alpha();
	void override_fov(view_setup_t* view_setup);
	//void remove_smoke();
	void force_crosshair();

	void unlock_hidden_convars();
	void third_person();
}
