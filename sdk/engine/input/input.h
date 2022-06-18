#pragma once

#include <windows.h>

#include "../../source engine/vector.h"

enum e_mouse_button {
    MOUSE_LEFT,
    MOUSE_RIGHT,
    MOUSE_MIDDLE,
    MOUSE_SIDE1,
    MOUSE_SIDE2,
};

enum e_mouse_cursor {
    CURSOR_NONE = -1,
    CURSOR_ARROW = 0,
    CURSOR_TEXT_INPUT,
    CURSOR_RESIZE_ALL,
    CURSOR_RESIZE_NS,
    CURSOR_RESIZE_EW,
    CURSOR_RESIZE_NESW,
    CURSOR_RESIZE_NWSE,
    CURSOR_HAND,
    CURSOR_NOT_ALLOWED,
};

namespace input {
    void init(HWND window_handle);
    void undo();

    void set_cursor(int cursor);
    void set_can_change_cursor(bool state);

    float get_mouse_wheel();

    point_t get_mouse_pos();
    point_t get_mouse_delta();

    bool is_in_bounds(const point_t &min, const point_t &max);
    bool is_mouse_clicked(int key, bool repeat = false);
    bool is_mouse_double_clicked(int key);
    bool is_mouse_down(int key);
    bool is_mouse_released(int key);

    bool is_key_pressed(int key, bool repeat = true);
    bool is_key_down(int key);
    bool is_key_released(int key);

    float get_key_press_length(int key);
    float get_mouse_click_length(int key);
} // namespace input