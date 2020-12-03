#pragma once

#include <string>

#include "../core/patterns.hpp"
#include "color.hpp"
#include "macros.hpp"
#include "typedefs.hpp"
#include "vertex.hpp"

enum e_font_flag {
	FONT_FLAG_NONE = 0x000,
	FONT_FLAG_ITALIC = 0x001,
	FONT_FLAG_UNDERLINE = 0x002,
	FONT_FLAG_STRIKEOUT = 0x004,
	FONT_FLAG_SYMBOL = 0x008,
	FONT_FLAG_ANTIALIAS = 0x010,
	FONT_FLAG_GAUSSIAN_BLUR = 0x020,
	FONT_FLAG_ROTARY = 0x040,
	FONT_FLAG_DROP_SHADOW = 0x080,
	FONT_FLAG_ADDITIVE = 0x100,
	FONT_FLAG_OUTLINE = 0x200,
	FONT_FLAG_CUSTOM = 0x400,
};

class c_vgui_surface {
	char padding_[0x280];

public:
	bool clipping_enabled;

	VIRTUAL_METHOD(void, set_draw_color, 15, (int r, int g, int b, int a = 255), r, g, b, a);
	VIRTUAL_METHOD(void, set_draw_color, 15, (const color_t& color), color.r, color.g, color.b, color.a);
	VIRTUAL_METHOD(void, fill_rectangle, 16, (int x, int y, int width, int height), x, y, x + width, y + height);
	VIRTUAL_METHOD(void, draw_rectangle, 18, (int x, int y, int width, int height), x, y, x + width, y + height);
	VIRTUAL_METHOD(void, draw_line, 19, (int x1, int y1, int x2, int y2), x1, y2, x2, y2);
	VIRTUAL_METHOD(void, set_text_font, 23, (vgui_font_t font), font);
	VIRTUAL_METHOD(void, set_text_color, 25, (int r, int g, int b, int a = 255), r, g, b, a);
	VIRTUAL_METHOD(void, set_text_color, 25, (const color_t& color), color.r, color.g, color.b, color.a);
	VIRTUAL_METHOD(void, set_text_pos, 26, (int x, int y), x, y);
	VIRTUAL_METHOD(void, render_text, 28, (std::wstring_view string, int length, int flags = 0), string.data(), length, flags);
	VIRTUAL_METHOD(void, update_texture, 37, (vgui_texture_t texture, const uint8_t* texture_data, int width, int height), texture, texture_data, width, height);
	VIRTUAL_METHOD(void, set_texture, 38, (vgui_texture_t texture), texture);
	VIRTUAL_METHOD(bool, delete_texture, 39, (vgui_texture_t texture), texture);
	VIRTUAL_METHOD(void, draw_textured_rect, 41, (int x, int y, int width, int height), x, y, x + width, y + height);
	VIRTUAL_METHOD(vgui_texture_t, create_texture, 43, (bool procedural = false), procedural);
	VIRTUAL_METHOD(void, unlock_cursor, 66, ());
	VIRTUAL_METHOD(void, lock_cursor, 67, ());
	VIRTUAL_METHOD(vgui_font_t, create_font, 71, ());
	VIRTUAL_METHOD(void, set_font_glyph_set, 72, (vgui_font_t font, std::string_view font_name, int size, int weight, int blur, int scanlines, int flags), font, font_name.data(), size, weight, blur, scanlines, flags, 0, 0);
	VIRTUAL_METHOD(void, calculate_text_size, 79, (vgui_font_t font, std::wstring_view string, int& width, int& height), font, string.data(), &width, &height);
	VIRTUAL_METHOD(void, draw_textured_polyline, 104, (const vertex_t* vertices, int count), vertices, count);
	VIRTUAL_METHOD(void, draw_textured_polygon, 106, (const vertex_t* vertices, int count), count, vertices, true);
	VIRTUAL_METHOD(void, fill_faded_rectangle, 123, (int x, int y, int width, int height, int start_alpha, int end_alpha, bool horizontal), x, y, x + width, y + height, start_alpha, end_alpha, horizontal);
	VIRTUAL_METHOD(void, get_clip_rect, 146, (int& x1, int& y1, int& x2, int& y2), &x1, &y1, &x2, &y2);
	VIRTUAL_METHOD(void, set_clip_rect, 147, (int x1, int y1, int x2, int y2), x1, y1, x2, y2);
	VIRTUAL_METHOD(void, set_alpha_multiplier, 113, (float alpha), alpha);
	VIRTUAL_METHOD(float, get_alpha_multiplier, 114, ());

	void start_drawing() {
		return reinterpret_cast<void(__thiscall*)(c_vgui_surface*)>(patterns::engine_vgui_start_drawing)(this);
	}

	void finish_drawing() {
		return reinterpret_cast<void(__thiscall*)(c_vgui_surface*)>(patterns::engine_vgui_finish_drawing)(this);
	}
};
