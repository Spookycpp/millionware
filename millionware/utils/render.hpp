#pragma once

#include <string>

#include "../sdk/color.hpp"
#include "../sdk/vector.hpp"

enum class e_font
{
  SEGOE_UI_14,
  SEGOE_UI_16,

  MAX,
};

enum class e_texture
{
  WHITE,

  MAX,
};

enum e_corner
{
  CORNER_NONE = 0,

  CORNER_TOP_LEFT = 1 << 0,
  CORNER_TOP_RIGHT = 1 << 1,
  CORNER_BOTTOM_LEFT = 1 << 2,
  CORNER_BOTTOM_RIGHT = 1 << 3,

  CORNER_TOP = CORNER_TOP_LEFT | CORNER_TOP_RIGHT,
  CORNER_RIGHT = CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
  CORNER_BOTTOM = CORNER_BOTTOM_LEFT | CORNER_BOTTOM_RIGHT,
  CORNER_LEFT = CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT,

  CORNER_ALL = CORNER_TOP | CORNER_RIGHT | CORNER_BOTTOM | CORNER_LEFT,
};

namespace render
{
  void initialize();
  void shutdown();

  void refresh_fonts();

  void reset_clip();
  void set_clip(int x1, int y1, int x2, int y2);
  void set_clip(const point_t& position, const point_t& size);

  void line(int x1, int y1, int x2, int y2, const color_t& color);
  void rect(int x, int y, int width, int height, const color_t& color);
  void rect_rounded(int x, int y, int width, int height, int radius, int corners, const color_t& color);
  void fill_rect(int x, int y, int width, int height, const color_t& color);
  void fill_rect_rounded(int x, int y, int width, int height, int radius, int corners, const color_t& color);
  void gradient_h(int x, int y, int width, int height, const color_t& start_color, const color_t& end_color);
  void gradient_v(int x, int y, int width, int height, const color_t& start_color, const color_t& end_color);
  void gradient_h_rounded(int x, int y, int width, int height, int radius, int corners, const color_t& start_color, const color_t& end_color);
  void gradient_v_rounded(int x, int y, int width, int height, int radius, int corners, const color_t& start_color, const color_t& end_color);
  void texture(int x, int y, int width, int height, e_texture texture, const color_t& color);
  void triangle(int x1, int y1, int x2, int y2, int x3, int y3, const color_t& color);
  void triangle_outline(int x1, int y1, int x2, int y2, int x3, int y3, const color_t& color);
  void text(int x, int y, e_font font, const color_t& color, std::wstring_view string);
  void text(int x, int y, e_font font, const color_t& color, std::string_view string);

  void line(const point_t& start, const point_t& end, const color_t& color);
  void rect(const point_t& position, const point_t& size, const color_t& color);
  void rect_rounded(const point_t& position, const point_t& size, int radius, int corners, const color_t& color);
  void fill_rect(const point_t& position, const point_t& size, const color_t& color);
  void fill_rect_rounded(const point_t& position, const point_t& size, int radius, int corners, const color_t& color);
  void gradient_h(const point_t& position, const point_t& size, const color_t& start_color, const color_t& end_color);
  void gradient_v(const point_t& position, const point_t& size, const color_t& start_color, const color_t& end_color);
  void gradient_h_rounded(const point_t& position, const point_t& size, int radius, int corners, const color_t& start_color, const color_t& end_color);
  void gradient_v_rounded(const point_t& position, const point_t& size, int radius, int corners, const color_t& start_color, const color_t& end_color);
  void texture(const point_t& position, const point_t& size, e_texture texture, const color_t& tint);
  void triangle(const point_t& point1, const point_t& point2, const point_t& point3, const color_t& color);
  void triangle_outline(const point_t& point1, const point_t& point2, const point_t& point3, const color_t& color);
  void text(const point_t& position, e_font font, const color_t& color, std::wstring_view string);
  void text(const point_t& position, e_font font, const color_t& color, std::string_view string);

  point_t measure_text(e_font font, std::wstring_view string);
  point_t measure_text(e_font font, std::string_view string);
};
