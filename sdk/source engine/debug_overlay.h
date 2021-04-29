#pragma once

#include "color.h"
#include "macros.h"
#include "vector.h"

class c_debug_overlay {

	DECLARE_VFUNC(13, screen_position(const vector_t& in, vector_t& out), int(__thiscall*)(void*, const vector_t& in, vector_t& out))(in, out);

public:

	DECLARE_VFUNC(5, add_line(const vector_t& from, const vector_t& to, const color_t& color, bool disable_depth_test, float duration), void(__thiscall*)(void*, const vector_t&, const vector_t&, int, int, int, bool, float))(from, to, color.r, color.g, color.b, disable_depth_test, duration);
	DECLARE_VFUNC(9, add_swept_box(const vector_t& start, const vector_t& end, const vector_t& mins, const vector_t& maxs, const vector_t& angles, const color_t& color, float duration), void(__thiscall*)(void*, const vector_t&, const vector_t&, const vector_t&, const vector_t&, const vector_t&, int, int, int, int, float))(start, end, mins, maxs, angles, color.r, color.g, color.b, color.a, duration);

	point_t screen_position(const vector_t& world_pos) {
		vector_t screen_pos;

		if (screen_position(world_pos, screen_pos) != 1)
			return { screen_pos.x, screen_pos.y };

		return { 0.0f, 0.0f };
	}
};