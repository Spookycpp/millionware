#pragma once

#include "vector.hpp"

struct vertex_t {
	vector2_t position;
	vector2_t texture_coord;

	constexpr vertex_t(vector2_t position, vector2_t texture_coord)
		: position(position), texture_coord(texture_coord) {
	}

	constexpr vertex_t()
		: vertex_t(0, 0) {}
};
