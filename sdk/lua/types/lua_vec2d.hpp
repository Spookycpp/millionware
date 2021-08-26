#pragma once

#include "../../source engine/vector.h"

namespace lua_internal::types {
    class vec2d {
    public:
        float x, y;

        vec2d() : x{ 0.0f }, y{ 0.0f } {}
        vec2d(const float x, const float y) : x{ x }, y{ y } {}

        vec2d &operator=(point_t &point) {
            x = point.x;
            y = point.y;
            return *this;
        }

        vec2d operator+(const vec2d &v) const {
            return vec2d(x + v.x, y + v.y);
        }

        vec2d operator-(const vec2d &v) const {
            return vec2d(x - v.x, y - v.y);
        }

        vec2d mul(const vec2d &vec) const {
            return vec2d(x * vec.x, y * vec.y);
        }

        vec2d operator*(const float &fl) const {
            return vec2d(x * fl, y * fl);
        }

        vec2d operator/(const vec2d &vec) const {
            return vec2d(x / vec.x, y / vec.y);
        }

        vec2d &operator/=(const float &fl) {
            const float a = 1.0f / fl;

            x *= a;
            y *= a;

            return *this;
        }

        float length() const {
            return sqrt(x * x + y * y);
        }

        float length_sqr() const {
            return x * x + y * y;
        }

        float dot(const vec2d &vec) const {
            return x * vec.x + y * vec.y;
        }

        float dist(const vec2d &vec) const {
            vec2d delta;

            delta.x = x - vec.x;
            delta.y = y - vec.y;

            return delta.length();
        }

        void normalize() {
            *this /= length();
        }

        vec2d lerp(const vec2d &to, const float t) {
            return to * t + *this * (1.0f - t);
        }

        bool empty() {
            return x == 0.0f && y == 0.0f;
        }

        bool valid() {
            return std::isfinite(x) && std::isfinite(y);
        }

        void zero() {
            this->x = 0;
            this->y = 0;
        }
    };
}

inline void lua_internal::context::lua_vec2d() const {
    auto lua = sol::state_view(l);

    auto vec2d_table = lua.new_usertype<types::vec2d>("vec2d",
        sol::call_constructor, sol::factories([](const float &x, const float &y) {
            return std::make_shared<types::vec2d>(x, y);
        })
    );

    vec2d_table["x"] = &types::vec2d::x;
    vec2d_table["y"] = &types::vec2d::y;

    vec2d_table["__add"] = &types::vec2d::operator+;
    vec2d_table["__sub"] = &types::vec2d::operator-;
    vec2d_table["__mul"] = &types::vec2d::mul;
    vec2d_table["__div"] = &types::vec2d::operator/;
    vec2d_table["__len"] = &types::vec2d::length;

    vec2d_table["length"] = &types::vec2d::length;
    vec2d_table["length_sqr"] = &types::vec2d::length_sqr;
    vec2d_table["dot"] = &types::vec2d::dot;
    vec2d_table["normalize"] = &types::vec2d::normalize;
    vec2d_table["lerp"] = &types::vec2d::lerp;
    vec2d_table["empty"] = &types::vec2d::empty;
    vec2d_table["valid"] = &types::vec2d::valid;
    vec2d_table["zero"] = &types::vec2d::zero;
}