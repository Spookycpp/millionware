//#pragma once
//
//#include <functional>
//#include <string>
//
//#include "../../source engine/color.h"
//#include "../../source engine/vector.h"
//
//enum e_surface_font {
//    SFONT_TAHOMA_11,
//    SFONT_SMALL_TEXT,
//
//    SFONT_MAX,
//};
//
//namespace surface {
//
//    void initialize();
//    void refresh_fonts();
//
//    void draw_line(const point_t &start, const point_t &end, const color_t &color);
//    void draw_rect(const point_t &position, const point_t &size, const color_t &color);
//    void fill_rect(const point_t &position, const point_t &size, const color_t &color);
//    void gradient_h(const point_t &position, const point_t &size, const color_t &start_color, const color_t &end_color);
//    void gradient_v(const point_t &position, const point_t &size, const color_t &start_color, const color_t &end_color);
//    void draw_text(const point_t &position, const color_t &color, std::string_view string, int font);
//
//    point_t measure_text(std::string_view string, int font);
//} // namespace surface