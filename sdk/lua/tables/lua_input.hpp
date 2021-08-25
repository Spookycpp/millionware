#pragma once

#include "../lua_callback.hpp"
#include "../lua_internal.hpp"

#include "../../core/cheat/cheat.h"
#include "../../engine/input/input.h"

namespace lua_internal::tables::lua_input {
    inline bool button_down(lua_State *l) {
        return input::is_key_down(luaL_checkinteger(l, 1));
    }

    inline bool button_pressed(lua_State *l) {
        return input::is_key_pressed(luaL_checkinteger(l, 1));
    }

    inline bool button_released(lua_State *l) {
        return input::is_key_released(luaL_checkinteger(l, 1));
    }

    inline float get_button_down_length(lua_State *l) {
        const int key = luaL_checkinteger(l, 1);
        if (key > 0x06) {
            return input::get_key_press_length(key);
        }

        switch (key) {
        case VK_LBUTTON:
            return input::get_mouse_click_length(MOUSE_LEFT);
        case VK_MBUTTON:
            return input::get_mouse_click_length(MOUSE_MIDDLE);
        case VK_RBUTTON:
            return input::get_mouse_click_length(MOUSE_RIGHT);
        case VK_XBUTTON1:
            return input::get_mouse_click_length(MOUSE_SIDE1);
        case VK_XBUTTON2:
            return input::get_mouse_click_length(MOUSE_SIDE2);
        default: break;
        }

        return 0.0f;
    }

    inline bool mouse_in_bounds(lua_State *l) {
        const auto min = user_data_argument<vec2d, point_t>(l, 1);
        const auto max = user_data_argument<vec2d, point_t>(l, 2);

        return input::is_in_bounds(min, max);
    }

    inline vec2d get_mouse_position(lua_State *l) {
        return { input::get_mouse_pos().x, input::get_mouse_pos().y };
    }

    inline float get_scroll_wheel(lua_State *l) {
        return input::get_mouse_wheel();
    }
} // namespace lua_internal::tables::input

inline void lua_internal::context::lua_input() {
    /*luabridge::getGlobalNamespace(l)
        .beginNamespace(xs("input"))
        .addFunction(xs("button_down"), std::function([this]() { return tables::lua_input::button_down(l); }))
        .addFunction(xs("button_pressed"), std::function([this]() { return tables::lua_input::button_pressed(l); }))
        .addFunction(xs("button_released"), std::function([this]() { return tables::lua_input::button_released(l); }))
        .addFunction(xs("get_button_down_length"), std::function([this]() { return tables::lua_input::get_button_down_length(l); }))
        .addFunction(xs("mouse_in_bounds"), std::function([this]() { return tables::lua_input::mouse_in_bounds(l); }))
        .addFunction(xs("get_mouse_position"), std::function([this]() { return tables::lua_input::get_mouse_position(l); }))
        .addFunction(xs("get_scroll_wheel"), std::function([this]() { return tables::lua_input::get_scroll_wheel(l); }))

        .endNamespace();*/
}