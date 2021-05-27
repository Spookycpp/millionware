#pragma once

#include <d3d9.h>

#include "../../source engine/color.h"
#include "../../source engine/vector.h"

enum e_font
{
	FONT_VERDANA_12,
    FONT_VERDANA_14,
	FONT_VERDANA_24,
    FONT_TAHOMA_11,
	FONT_TAHOMA_12,
	FONT_SMALL_TEXT,
	FONT_CEREBRI_SANS_BOLD_13,
	FONT_CEREBRI_SANS_MEDIUM_14,
	FONT_CEREBRI_SANS_MEDIUM_18,
	FONT_CEREBRI_SANS_BOLD_32,
	FONT_WEAPONS_16,
	FONT_WEAPONS_32,
	FONT_FA_BRANDS_32,
	FONT_FA_REGULAR_32,
	FONT_FA_SOLID_32,

	// marks the last font available and is to tell how many fonts there are
	// if you add any new fonts please put it BEFORE this one or else u might cause heap corruption
	FONT_MAX,
};

enum e_texture
{
	TEXTURE_MW_LOGO_BASE,
	TEXTURE_MW_LOGO_DOLLAR,
	TEXTURE_TRANSPARENCY,

	// same here as with fonts
	TEXTURE_MAX,
};

enum e_corner
{
	CORNER_NONE = 0,
	CORNER_TOP_LEFT = 1 << 0,
	CORNER_TOP_RIGHT = 1 << 1,
	CORNER_BOTTOM_LEFT = 1 << 2,
	CORNER_BOTTOM_RIGHT = 1 << 3,
	CORNER_TOP = CORNER_TOP_LEFT | CORNER_TOP_RIGHT,
	CORNER_BOTTOM = CORNER_BOTTOM_LEFT | CORNER_BOTTOM_RIGHT,
	CORNER_LEFT = CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT,
	CORNER_RIGHT = CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
	CORNER_ALL = CORNER_TOP | CORNER_BOTTOM,
};

namespace render
{
	void init(HWND window, IDirect3DDevice9 *device);
	void undo();

	float get_frame_time();

	point_t get_screen_size();

	void begin();
	void finish();
	void set_layer(int index);

	void push_clip(const point_t &position, const point_t &size, bool intersect = true);
	void reset_clip();
	void pop_clip();

	void draw_line(const point_t &start, const point_t &end, const color_t &color);
	void draw_rect(const point_t &position, const point_t &size, const color_t &color, float rounding = 0.0f, int corners = CORNER_ALL);
	void fill_rect(const point_t &position, const point_t &size, const color_t &color, float rounding = 0.0f, int corners = CORNER_ALL);
	void gradient_h(const point_t &position, const point_t &size, const color_t &color_start, const color_t &color_end);
	void gradient_v(const point_t &position, const point_t &size, const color_t &color_start, const color_t &color_end);
	void draw_triangle(const point_t &point1, const point_t &point2, const point_t &point3, const color_t &color);
	void fill_triangle(const point_t &point1, const point_t &point2, const point_t &point3, const color_t &color);
	void draw_circle(const point_t &center, float radius, const color_t &color, int segments = 0);
	void fill_circle(const point_t &center, float radius, const color_t &color, int segments = 0);
	void draw_text(const point_t &position, const color_t &color, const char *text, int font, float wrap_width = 0.0f, float font_size = -1.0f);
	void draw_image(const point_t &position, const point_t &size, const color_t &color, int texture, float rounding = 0.0f, int corners = CORNER_ALL);

	point_t measure_text(const char *text, int font, float wrap_width = 0.0f, float font_size = -1.0f);
}
