#pragma once
#include <utility>

#include "../../sdk/entity.hpp"
#include "../../sdk/vector.hpp"

namespace features::hit_chance
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