#pragma once

namespace lua_internal::types {
    class vec3d {
    public:
        float x, y, z;

        vec3d() : x{ 0.0f }, y{ 0.0f }, z{ 0.0f } {}
        vec3d(const float x, const float y, const float z) : x{ x }, y{ y }, z{ z } {}

        vec3d operator+(const vec3d &v) const {
            return vec3d(x + v.x, y + v.y, z + v.z);
        }

        vec3d operator-(const vec3d &v) const {
            return vec3d(x - v.x, y - v.y, z - v.z);
        }

        vec3d mul(const vec3d &v) const {
            return vec3d(x * v.x, y * v.y, z * v.z);
        }

        vec3d operator*(const float &fl) const {
            return vec3d(x * fl, y * fl, z * fl);
        }

        vec3d operator/(const vec3d &v) const {
            return vec3d(x / v.x, y / v.y, z / v.z);
        }

        vec3d &operator/=(const float &fl) {
            const float a = 1.0f / fl;

            x *= a;
            y *= a;
            z *= a;

            return *this;
        }

        float length() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        float length_2d() const {
            return std::sqrtf(x * x + y * y);
        }

        float length_sqr() const {
            return x * x + y * y + z * z;
        }

        float length_2d_sqr() const {
            return x * x + y * y;
        }

        float dot(const vec3d &v) const {
            return x * v.x + y * v.y + z * v.z;
        }

        float dist(const vec3d &v) const {
            vec3d delta;

            delta.x = x - v.x;
            delta.y = y - v.y;
            delta.z = z - v.z;

            return delta.length();
        }

        void normalize() {
            *this /= length();
        }

        vec3d lerp(const vec3d &to, const float t) {
            return to * t + *this * (1.0f - t);
        }

        bool empty() {
            return x == 0.0f && y == 0.0f && z == 0.0f;
        }

        bool valid() {
            return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
        }

        void zero() {
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }
    };
}

inline void lua_internal::context::lua_vec3d() const {
    auto lua = sol::state_view(l);

    auto vec3d_table = lua.new_usertype<types::vec3d>("vec3d",
        sol::call_constructor, sol::factories([](const float &x, const float &y, const float &z) {
            return std::make_shared<types::vec3d>(x, y, z);
        })
    );

    vec3d_table["x"] = &types::vec3d::x;
    vec3d_table["y"] = &types::vec3d::y;
    vec3d_table["z"] = &types::vec3d::z;
    
    vec3d_table["__add"] = &types::vec3d::operator+;
    vec3d_table["__sub"] = &types::vec3d::operator-;
    vec3d_table["__mul"] = &types::vec3d::mul;
    vec3d_table["__div"] = &types::vec3d::operator/;
    vec3d_table["__len"] = &types::vec3d::length;
    
    vec3d_table["length"] = &types::vec3d::length;
    vec3d_table["length_sqr"] = &types::vec3d::length_sqr;
    vec3d_table["length_2d"] = &types::vec3d::length_2d;
    vec3d_table["length_2d_sqr"] = &types::vec3d::length_2d_sqr;
    vec3d_table["dot"] = &types::vec3d::dot;
    vec3d_table["dist"] = &types::vec3d::dist;
    vec3d_table["normalize"] = &types::vec3d::normalize;
    vec3d_table["lerp"] = &types::vec3d::lerp;
    vec3d_table["empty"] = &types::vec3d::empty;
    vec3d_table["valid"] = &types::vec3d::valid;
    vec3d_table["zero"] = &types::vec3d::zero;
}
