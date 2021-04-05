#pragma once

#include <cstdint>

#define PATTERN_GET(p, a) inline auto get_##p() { return p == 0u ? 0u : (a); }

class c_player;

namespace patterns
{
	inline uintptr_t input;
	inline uintptr_t local_player;
	inline uintptr_t view_matrix;
	inline uintptr_t d3d9_device;
	inline uintptr_t set_clantag;
	inline uintptr_t client_mode;
	inline uintptr_t client_state;
	inline uintptr_t global_vars;
	inline uintptr_t is_demo_or_hltv;
	inline uintptr_t money_reveal;
	inline uintptr_t insert_into_tree_list_leaves_in_box_call;
	inline uintptr_t weapon_system;
	inline uintptr_t flash_time;
	inline uintptr_t line_goes_through_smoke;
	inline uintptr_t move_helper;
	inline uintptr_t accept_match;

	PATTERN_GET(input, *(uintptr_t *) (input + 1u));
	PATTERN_GET(local_player, **(c_player ***) (local_player + 2u));
	PATTERN_GET(view_matrix, (*(uintptr_t *) (view_matrix + 3u)) + 176u);
	PATTERN_GET(d3d9_device, **(uintptr_t **) (d3d9_device + 1u));
	PATTERN_GET(set_clantag, set_clantag);
	PATTERN_GET(client_mode, **(uintptr_t **) (client_mode + 5u));
	PATTERN_GET(client_state, **(uintptr_t **) (client_state + 1u));
	PATTERN_GET(global_vars, **(uintptr_t **) (global_vars + 1u));
	PATTERN_GET(is_demo_or_hltv, is_demo_or_hltv);
	PATTERN_GET(money_reveal, money_reveal);
	PATTERN_GET(insert_into_tree_list_leaves_in_box_call, insert_into_tree_list_leaves_in_box_call);
	PATTERN_GET(weapon_system, *(uintptr_t **)(weapon_system + 2u));
	PATTERN_GET(flash_time, *(uint32_t*)(flash_time + 4u));
	PATTERN_GET(line_goes_through_smoke, *(uintptr_t*)(line_goes_through_smoke));
	PATTERN_GET(move_helper, **(uint32_t ***)(move_helper + 2u));
	PATTERN_GET(accept_match, accept_match);

	bool init();
}
