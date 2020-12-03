#pragma once

#include "../../sdk/user_cmd.hpp"

namespace features::misc {

	void bunny_hop(user_cmd_t* user_cmd);
	void infinite_duck(user_cmd_t* user_cmd);
	void auto_strafer(user_cmd_t* user_cmd);

	void auto_accept();
	void rank_reveal();
	void aspect_ratio();
	void clantag();
	void name_spam();
	void panoramic_blur();
	void post_processing();
	void remove_flash();
	void remove_smoke();
}
