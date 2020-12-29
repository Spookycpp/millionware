#pragma once

#include <utility>

#include "../../../sdk/vector.hpp"

namespace features::aimbot::hitchance
{
	struct hit_chance_data_t {
		std::pair< float, float > random;
		std::pair< float, float > inaccuracy;
		std::pair< float, float > spread;
	};

	void initialize();

	vector3_t get_spread_direction(c_weapon* wpn, const vector3_t& angles, int seed);

	bool can_hit(c_player* target, c_weapon* wpn, const vector3_t& angles, int percentage, int hitbox);
}