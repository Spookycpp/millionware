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
	inline uintptr_t move_helper = 0;
	inline uintptr_t global_vars = 0;
	inline uintptr_t client_mode = 0;
	inline uintptr_t line_goes_through_smoke = 0;
	inline uintptr_t flashbang_time = 0;
	inline uintptr_t clan_tag = 0;
	inline uintptr_t send_datagram = 0;
	inline uintptr_t client_state = 0;
	inline uintptr_t inventory_unlocker = 0;
	inline uintptr_t get_sequence_activity = 0;

	void initialize();

}