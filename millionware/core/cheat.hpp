#pragma once

#include <chrono>
#include <string_view>

#include "../sdk/entity.hpp"

namespace cheat {

	void initialize();

	// cheat variables
	uint64_t get_inject_time();
	uint64_t get_sub_expire();

	std::string_view get_username();

	// global stuff
	inline c_player* local_player;
	inline point_t screen_size;

	inline uint32_t unpredicted_flags;
	inline bool b_predicting;
	inline bool landed = false;
	inline float player_fov;
}