#pragma once

#include "macros.h"
#include "vector.h"

class c_debug_overlay
{
	DECLARE_VFUNC(13, screen_position(const vector_t& in, vector_t& out), int(__thiscall*)(void*, const vector_t& in, vector_t& out))(in, out);

public:
	point_t screen_position(const vector_t& world_pos)
	{
		vector_t screen_pos;

		if (screen_position(world_pos, screen_pos) != 1)
			return { screen_pos.x, screen_pos.y };

		return { 0.0f, 0.0f };
	}
};