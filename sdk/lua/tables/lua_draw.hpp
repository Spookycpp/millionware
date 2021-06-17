#pragma once

#include "lua_entity.hpp"
#include "../lua_internal.hpp"
#include "../types/lua_vec2d.hpp"

#include "../../engine/render/render.h"
#include "../../engine/logging/logging.h"
#include "../../engine/security/xorstr.h"
#include "../../engine/math/math.h"

//@todo: CLEAN THIS UP

inline std::vector<IDirect3DTexture9 *> _textures;
inline std::vector<render::animated_gif> _gifs;

namespace lua_internal::tables::draw {
    template <class T1, class T2 = T1>
    T2 user_data_argument(lua_State *l, const int arg, const bool can_be_const = true) {
        if (!luabridge::LuaRef::fromStack(l, arg).isUserdata()) {
            return {};
        }

        return *reinterpret_cast<T2 *>(luabridge::detail::Userdata::get<T1>(l, arg, can_be_const));
    }

    inline void text(lua_State *l) {
        size_t len;

        const auto pos = user_data_argument<vec2d, point_t>(l, 1);
        const auto col = user_data_argument<color, color_t>(l, 2);

        render::draw_text(pos, col, luaL_checklstring(l, 3, &len), FONT_TAHOMA_11, 0);
    }

    inline void wrap_text(lua_State *l) {
        size_t len;

        const auto pos = user_data_argument<vec2d, point_t>(l, 1);
        const auto col = user_data_argument<color, color_t>(l, 2);

        render::draw_text(pos, col, luaL_checklstring(l, 4, &len), FONT_TAHOMA_11, 
            static_cast<float>(luaL_checknumber(l, 3)));
    }

    inline vec2d text_size(lua_State *l) {
        size_t len;

        const point_t size = render::measure_text(luaL_checklstring(l, 1, &len), FONT_TAHOMA_11, 
            static_cast<float>(luaL_checkinteger(l, 2)), static_cast<float>(luaL_checkinteger(l, 3)));

        return vec2d{size.x, size.y};
    }

    inline void line(lua_State *l) {
        const auto pos1 = user_data_argument<vec2d, point_t>(l, 1);
        const auto pos2 = user_data_argument<vec2d, point_t>(l, 2);
        const auto col = user_data_argument<color, color_t>(l, 3);

        render::draw_line(pos1, pos2, col, static_cast<float>(luaL_checknumber(l, 4)));
    }

    inline void rect(lua_State *l) {
        const auto pos = user_data_argument<vec2d, point_t>(l, 1);
        const auto size = user_data_argument<vec2d, point_t>(l, 2);
        const auto col = user_data_argument<color, color_t>(l, 3);

        render::draw_rect(pos, size, col, static_cast<float>(luaL_checknumber(l, 4)));
    }

    inline void fill_rect(lua_State *l) {
        const auto pos = user_data_argument<vec2d, point_t>(l, 1);
        const auto size = user_data_argument<vec2d, point_t>(l, 2);
        const auto col = user_data_argument<color, color_t>(l, 3);

        render::fill_rect(pos, size, col, static_cast<float>(luaL_checknumber(l, 4)));
    }

    inline void gradient(lua_State *l) {
        const auto pos = user_data_argument<vec2d, point_t>(l, 1);
        const auto size = user_data_argument<vec2d, point_t>(l, 2);
        const auto col1 = user_data_argument<color, color_t>(l, 3);
        const auto col2 = user_data_argument<color, color_t>(l, 4);

        if (lua_toboolean(l, 5)) {
            render::gradient_h(pos, size, col1, col2);
            return;
        }

        render::gradient_v(pos, size, col1, col2);
    }

    inline void triangle(lua_State *l) {
        const auto pos1 = user_data_argument<vec2d, point_t>(l, 1);
        const auto pos2 = user_data_argument<vec2d, point_t>(l, 2);
        const auto pos3 = user_data_argument<vec2d, point_t>(l, 3);
        const auto col = user_data_argument<color, color_t>(l, 4);

        render::draw_triangle(pos1, pos2, pos3, col);
    }

    inline void fill_triangle(lua_State *l) {
        const auto pos1 = user_data_argument<vec2d, point_t>(l, 1);
        const auto pos2 = user_data_argument<vec2d, point_t>(l, 2);
        const auto pos3 = user_data_argument<vec2d, point_t>(l, 3);
        const auto col = user_data_argument<color, color_t>(l, 4);

        render::fill_triangle(pos1, pos2, pos3, col);
    }

    inline void circle(lua_State *l) {
        const auto pos = user_data_argument<vec2d, point_t>(l, 1);
        const auto col = user_data_argument<color, color_t>(l, 2);

        render::draw_circle(pos, static_cast<float>(luaL_checknumber(l, 3)), col, luaL_checkinteger(l, 4));
    }

    inline void fill_circle(lua_State *l) {
        const auto pos = user_data_argument<vec2d, point_t>(l, 1);
        const auto col = user_data_argument<color, color_t>(l, 2);

        render::draw_circle(pos, static_cast<float>(luaL_checknumber(l, 3)), col, luaL_checkinteger(l, 4));
    }

    inline void bezier_cubic(lua_State *l) {
        const auto pos1 = user_data_argument<vec2d, point_t>(l, 1);
        const auto pos2 = user_data_argument<vec2d, point_t>(l, 2);
        const auto pos3 = user_data_argument<vec2d, point_t>(l, 3);
        const auto pos4 = user_data_argument<vec2d, point_t>(l, 4);
        const auto col = user_data_argument<color, color_t>(l, 5);

        render::draw_bezier_cubic(pos1, pos2, pos3, pos4, col, static_cast<float>(luaL_checkinteger(l, 6)));
    }

    inline void bezier_quad(lua_State *l) {
        const auto pos1 = user_data_argument<vec2d, point_t>(l, 1);
        const auto pos2 = user_data_argument<vec2d, point_t>(l, 2);
        const auto pos3 = user_data_argument<vec2d, point_t>(l, 3);
        const auto col = user_data_argument<color, color_t>(l, 4);

        render::draw_bezier_quad(pos1, pos2, pos3, col, static_cast<float>(luaL_checkinteger(l, 5)));
    }

    inline void poly_chain(lua_State *l) {
        if (!luabridge::LuaRef::fromStack(l, 1).isTable()) {
            return;
        }

        std::vector<point_t> points;

        lua_pushvalue(l, 1); // -1 table
        lua_pushnil(l); // -1 nil, -2 table

        while (lua_next(l, -2)) {
            // -1 value, -2 key, -3 table
            lua_pushvalue(l, -2);
            // -1 key, -2 value, -3 key, -4 table
            if (luabridge::LuaRef::fromStack(l, -2).isUserdata()) {
                if (const auto *ptr = reinterpret_cast<point_t *>(
                    luabridge::detail::Userdata::get<vec2d>(l, -2, true)); ptr) {
                    points.push_back(*ptr);
                }
            }

            // pop key and value
            lua_pop(l, 2);
        }

        // pop table
        lua_pop(l, 1);

        if (points.empty()) {
            return;
        }

        const auto col = user_data_argument<color, color_t>(l, 2);
        render::draw_poly_line(points.data(), points.size(), col, static_cast<float>(luaL_checknumber(l, 3)));   
    }

    //@todo: redo this
    inline size_t load_image(lua_State *l) {
        if (_textures.size() > 500) {
            // bruh stop
            return _textures.size();
        }

        size_t len;

        std::ifstream input(luaL_checklstring(l, 1, &len), std::ios::binary);
        std::vector<uint8_t> bytes((std::istreambuf_iterator(input)), (std::istreambuf_iterator<char>()));
        input.close();

        IDirect3DTexture9 *texture = render::create_from_png(bytes.data(), bytes.size());
        if (!texture) {
            return 0;
        }

        _textures.push_back(texture);
        return _textures.size() - 1;
    }

    //@todo: redo this
    inline size_t load_svg(lua_State *l) {
        if (_textures.size() > 500) {
            return _textures.size();
        }

        size_t len;

        std::ifstream input(luaL_checklstring(l, 1, &len), std::ios::binary);
        std::vector bytes((std::istreambuf_iterator(input)), (std::istreambuf_iterator<char>()));
        input.close();

        IDirect3DTexture9 *texture = render::rasterize_vector(bytes.data(), 
            static_cast<float>(luaL_checknumber(l, 2)));
        if (!texture) {
            return 0;
        }

        _textures.push_back(texture);
        return _textures.size() - 1;
    }

    //@todo: redo this
    inline size_t load_svg_raw(lua_State *l) {
        if (_textures.size() > 500) {
            return _textures.size();
        }

        size_t len;

        const auto str = luaL_checklstring(l, 1, &len);

        std::vector<char> bytes(len);
        const auto n = std::strlen(str);
        std::memcpy(bytes.data(), str, n);

        IDirect3DTexture9 *texture = render::rasterize_vector(bytes.data(), 
            static_cast<float>(luaL_checknumber(l, 2)));
        if (!texture) {
            return 0;
        }

        _textures.push_back(texture);
        return _textures.size() - 1;
    }

    //@todo: redo this
    inline size_t load_gif(lua_State *l) {
        if (_gifs.size() > 500) {
            return _gifs.size();
        }

        size_t len;

        std::ifstream input(luaL_checklstring(l, 1, &len), std::ios::binary);
        std::vector<uint8_t> bytes((std::istreambuf_iterator(input)), (std::istreambuf_iterator<char>()));
        input.close();

        _gifs.emplace_back(bytes.data(), bytes.size());
        return _gifs.size() - 1;
    }

    //@todo: redo this
    inline void image(lua_State *l) {
        if (_textures.empty()) {
            logging::error(XORSTR("no images have been loaded"));
            return;
        }

        const size_t idx = luaL_checkinteger(l, 5);
        if (idx > _textures.size()) {
            logging::error(XORSTR("image index of {} is out of bounds, current size is {}"), 
                idx, _textures.size());
            return;
        }

        const auto pos = user_data_argument<vec2d, point_t>(l, 1);
        const auto size = user_data_argument<vec2d, point_t>(l, 2);
        const auto col = user_data_argument<color, color_t>(l, 3);

        render::draw_image(pos, size, col, _textures.at(idx), 
            static_cast<float>(luaL_checknumber(l, 4)), CORNER_ALL);
    }

    //@todo: redo this
    inline void gif(lua_State *l) {
        if (_gifs.empty()) {
            logging::error(XORSTR("no gifs have been loaded"));
            return;
        }

        const size_t idx = luaL_checkinteger(l, 5);
        if (idx > _gifs.size()) {
            logging::error(XORSTR("gif index of {} is out of bounds, current size is {}"), idx, _gifs.size());
            return;
        }

        const auto pos = user_data_argument<vec2d, point_t>(l, 1);
        const auto size = user_data_argument<vec2d, point_t>(l, 2);
        const auto col = user_data_argument<color, color_t>(l, 3);

        _gifs.at(idx).draw(pos, size, col, 
            static_cast<float>(luaL_checknumber(l, 4)), CORNER_ALL);
    }

    inline vec2d world_to_screen(lua_State *l) {
        const auto world = user_data_argument<vec3d, vector_t>(l, 1);

        point_t point;
        if (!math::world_to_screen(world, point)) {
            return {0, 0};
        }

        return {point.x, point.y};
    }
} // namespace lua_internal::draw

inline void lua_internal::context::lua_draw() {
    static bool once = false;

    //@todo: srsly redo this
    if (!once) {
        _gifs.emplace_back(nullptr, 0);
        _textures.push_back(nullptr);
        once = true;
    }

    luabridge::getGlobalNamespace(l)
    .beginNamespace("draw")
        .addFunction("text", std::function([this]() { tables::draw::text(l); }))
        .addFunction("wrap_text", std::function([this]() { tables::draw::wrap_text(l); }))
        .addFunction("text_size", std::function([this]() { return tables::draw::text_size(l); }))
        .addFunction("line", std::function([this]() { tables::draw::line(l); }))
        .addFunction("rect", std::function([this]() { tables::draw::rect(l); }))
        .addFunction("fill_rect", std::function([this]() { tables::draw::fill_rect(l); }))
        .addFunction("gradient", std::function([this]() { tables::draw::gradient(l); }))
        .addFunction("triangle", std::function([this]() { tables::draw::triangle(l); }))
        .addFunction("fill_triangle", std::function([this]() { tables::draw::fill_triangle(l); }))
        .addFunction("circle", std::function([this]() { tables::draw::circle(l); }))
        .addFunction("fill_circle", std::function([this]() { tables::draw::fill_circle(l); }))
        .addFunction("bezier_cubic", std::function([this]() { tables::draw::bezier_cubic(l); }))
        .addFunction("bezier_quad", std::function([this]() { tables::draw::bezier_quad(l); }))
        .addFunction("poly_chain", std::function([this]() { tables::draw::poly_chain(l); }))
        .addFunction("load_image", std::function([this]() { return tables::draw::load_image(l); }))
        .addFunction("load_svg", std::function([this]() { return tables::draw::load_svg(l); }))
        .addFunction("load_svg_raw", std::function([this]() { return tables::draw::load_svg_raw(l); }))
        .addFunction("load_gif", std::function([this]() { return tables::draw::load_gif(l); }))
        .addFunction("image", std::function([this]() { tables::draw::image(l); }))
        .addFunction("gif", std::function([this]() { tables::draw::gif(l); }))
        .addFunction("world_to_screen", std::function([this]() { return tables::draw::world_to_screen(l); }))
    .endNamespace();
}
