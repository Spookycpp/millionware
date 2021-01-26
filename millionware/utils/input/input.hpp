#pragma once

#include "../../sdk/vector.hpp"
#include "../../sdk/vector.hpp"

namespace input {

	void update();

	point_t get_mouse_pos();

	bool is_key_pressed(int key);
	bool is_key_down(int key);
	bool is_key_released(int key);
	bool is_key_up(int key);

	bool is_in_bounds(int x1, int y1, int x2, int y2);
	bool is_in_bounds(const point_t& min, const point_t& max);

}
