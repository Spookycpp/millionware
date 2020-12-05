#pragma once

#include <cstdint>

namespace patterns {

	inline uintptr_t engine_vgui_start_drawing = 0;
	inline uintptr_t engine_vgui_finish_drawing = 0;
	inline uintptr_t player_has_bomb = 0;
	inline uintptr_t set_local_player_ready = 0;
	inline uintptr_t set_clantag = 0;
	inline uintptr_t weapon_system = 0;
	inline uintptr_t input = 0;
	inline uintptr_t demo_or_hltv = 0;
	inline uintptr_t money_reveal = 0;

	void initialize();

}
