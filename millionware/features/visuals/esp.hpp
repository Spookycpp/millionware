#pragma once

#include <optional>

#include "../../sdk/entity.hpp"
#include "../../sdk/vector.hpp"

struct screen_bounding_box_t {
	point_t min;
	point_t max;
};

namespace features::visuals::esp {

	std::optional<screen_bounding_box_t> get_entity_box(int index);

	void handle_player_esp(const screen_bounding_box_t& box, c_player* player);
	void handle_weapon_esp(const screen_bounding_box_t& box, c_weapon* weapon);

	void paint();

}
