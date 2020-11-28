#pragma once

#include <chrono>
#include <string_view>

#include "../sdk/entity.hpp"

using system_clock_time_point = std::chrono::time_point<std::chrono::system_clock>;

namespace cheat
{
	// cheat variables
	system_clock_time_point get_inject_time();
	system_clock_time_point get_sub_expire();

	std::string_view get_username();
	std::string_view get_access_token();
	std::string_view get_build_name();

	// global stuff
	inline point_t screen_size;
	inline c_player* local_player;
}
