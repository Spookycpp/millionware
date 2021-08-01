#include <array>
#include <stack>
#include <windows.h>

#include "surface.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../engine/security/xorstr.h"

static uint32_t fonts[SFONT_MAX];

constexpr uint32_t &get_font(int font) {
    return fonts[font];
}

void surface::initialize() {
    refresh_fonts();
}

void surface::refresh_fonts() {
    get_font(SFONT_TAHOMA_11) = interfaces::surface->create_font();
    get_font(SFONT_SMALL_TEXT) = interfaces::surface->create_font();

    interfaces::surface->set_font_glyph_set(get_font(SFONT_TAHOMA_11), xs("Tahoma"), 11, 500, FONT_FLAG_ANTIALIAS);
    interfaces::surface->set_font_glyph_set(get_font(SFONT_SMALL_TEXT), xs("Small Fonts"), 9, 400, FONT_FLAG_OUTLINE);
}

static void surface_line(int x1, int y1, int x2, int y2, const color_t &color) {
    interfaces::surface->set_draw_color(color.r, color.g, color.b, color.a);
    interfaces::surface->draw_line(x1, y1, x2, y2);
}

static void surface_rect(int x, int y, int width, int height, const color_t &color) {
    interfaces::surface->set_draw_color(color.r, color.g, color.b, color.a);
    interfaces::surface->draw_rectangle(x, y, width, height);
}

static void surface_fill_rect(int x, int y, int width, int height, const color_t &color) {
    interfaces::surface->set_draw_color(color.r, color.g, color.b, color.a);
    interfaces::surface->fill_rectangle(x, y, width, height);
}

static void surface_gradient_h(int x, int y, int width, int height, const color_t &start_color, const color_t &end_color) {
    interfaces::surface->set_draw_color(start_color.r, start_color.g, start_color.b, start_color.a);
    interfaces::surface->fill_faded_rectangle(x, y, width, height, 255, 255, true);

    interfaces::surface->set_draw_color(end_color.r, end_color.g, end_color.b, end_color.a);
    interfaces::surface->fill_faded_rectangle(x, y, width, height, 0, 255, true);
}

static void surface_gradient_v(int x, int y, int width, int height, const color_t &start_color, const color_t &end_color) {
    interfaces::surface->set_draw_color(start_color.r, start_color.g, start_color.b, start_color.a);
    interfaces::surface->fill_faded_rectangle(x, y, width, height, 255, 255, false);

    interfaces::surface->set_draw_color(end_color.r, end_color.g, end_color.b, end_color.a);
    interfaces::surface->fill_faded_rectangle(x, y, width, height, 0, 255, false);
}

static void surface_text(int x, int y, int font, const color_t &color, std::wstring_view string) {
    interfaces::surface->set_text_color(color.r, color.g, color.b, color.a);
    interfaces::surface->set_text_font(get_font(font));
    interfaces::surface->set_text_pos(x, y);
    interfaces::surface->render_text(string.data(), string.size(), 0);
}

static void surface_text(int x, int y, int font, const color_t &color, std::string_view string) {
    surface_text(x, y, font, color, std::wstring(string.begin(), string.end()));
}

void surface::draw_line(const point_t &start, const point_t &end, const color_t &color) {
    surface_line(start.x, start.y, end.x, end.y, color);
}

void surface::draw_rect(const point_t &position, const point_t &size, const color_t &color) {
    surface_rect(position.x, position.y, size.x, size.y, color);
}

void surface::fill_rect(const point_t &position, const point_t &size, const color_t &color) {
    surface_fill_rect(position.x, position.y, size.x, size.y, color);
}

void surface::gradient_h(const point_t &position, const point_t &size, const color_t &start_color, const color_t &end_color) {
    surface_gradient_h(position.x, position.y, size.x, size.y, start_color, end_color);
}

void surface::gradient_v(const point_t &position, const point_t &size, const color_t &start_color, const color_t &end_color) {
    surface_gradient_v(position.x, position.y, size.x, size.y, start_color, end_color);
}

void surface::draw_text(const point_t &position, const color_t &color, std::string_view string, int font) {
    surface_text(position.x, position.y, font, color, string);
}

point_t surface::measure_text(std::string_view string, int font) {
    auto wstring = std::wstring(string.begin(), string.end());
    int width, height;

    interfaces::surface->calculate_text_size(get_font(font), wstring.data(), width, height);

    return point_t(width, height);
}