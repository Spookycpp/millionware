#pragma once
#include "macros.hpp"

class c_engine_render_view
{
public:
	VIRTUAL_METHOD(void, set_blend, 4, (float blend), blend);
	VIRTUAL_METHOD(float, get_blend, 5, ());
	VIRTUAL_METHOD(void, set_color_modulation, 6, (float color[3]), color);
	VIRTUAL_METHOD(void, get_color_modulation, 7, (float color[3]), color);
};