#pragma once

#include <cstdint>
#include <string>

#include "macros.h"

#include "../core/patterns/patterns.h"

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
  public:
    DECLARE_VFUNC(15, set_draw_color(int r, int g, int b, int a), void(__thiscall *)(void *, int, int, int, int))(r, g, b, a);
    DECLARE_VFUNC(16, fill_rectangle(int x, int y, int width, int height), void(__thiscall *)(void *, int, int, int, int))(x, y, x + width, y + height);
    DECLARE_VFUNC(18, draw_rectangle(int x, int y, int width, int height), void(__thiscall *)(void *, int, int, int, int))(x, y, x + width, y + height);
    DECLARE_VFUNC(19, draw_line(int x1, int y1, int x2, int y2), void(__thiscall *)(void *, int, int, int, int))(x1, y1, x2, y2);
    DECLARE_VFUNC(23, set_text_font(uint32_t font), void(__thiscall *)(void *, uint32_t font))(font);
    DECLARE_VFUNC(25, set_text_color(int r, int g, int b, int a), void(__thiscall *)(void *, int, int, int, int))(r, g, b, a);
    DECLARE_VFUNC(26, set_text_pos(int x, int y), void(__thiscall *)(void *, int, int))(x, y);
    DECLARE_VFUNC(28, render_text(const wchar_t *string, int length, int flags), void(__thiscall *)(void *, const wchar_t *, int, int))(string, length, flags);
    DECLARE_VFUNC(66, unlock_cursor(), void(__thiscall *)(void *))();
    DECLARE_VFUNC(67, lock_cursor(), void(__thiscall *)(void *))();
    DECLARE_VFUNC(71, create_font(), uint32_t(__thiscall *)(void *))();
    DECLARE_VFUNC(72, set_font_glyph_set(uint32_t font, const char *font_name, int size, int weight, int flags), void(__thiscall *)(void *, uint32_t, const char *, int, int, int, int, int, int, int))
    (font, font_name, size, weight, 0, 0, flags, 0, 0);
    DECLARE_VFUNC(79, calculate_text_size(uint32_t font, const wchar_t *string, int &width, int &height), void(__thiscall *)(void *, uint32_t, const wchar_t *, int &, int &))
    (font, string, width, height);
    DECLARE_VFUNC(123, fill_faded_rectangle(int x, int y, int width, int height, int start_alpha, int end_alpha, bool horizontal), void(__thiscall *)(void *, int, int, int, int, int, int, bool))
    (x, y, x + width, y + height, start_alpha, end_alpha, horizontal);

    void start_drawing() {
        return reinterpret_cast<void(__thiscall *)(c_vgui_surface *)>(patterns::get_engine_vgui_start_drawing())(this);
    }

    void finish_drawing() {
        return reinterpret_cast<void(__thiscall *)(c_vgui_surface *)>(patterns::get_engine_vgui_finish_drawing())(this);
    }
};