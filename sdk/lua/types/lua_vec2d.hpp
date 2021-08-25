#pragma once

#include "../../source engine/vector.h"

class vec2d {
public:
	float x{}, y{};

	vec2d() : x{ 0.0f }, y{ 0.0f } {}
	vec2d(const float x, const float y) : x{ x }, y{ y } {}

    vec2d(const point_t &point) {
        *this = point;
    }

    vec2d &operator=(point_t &point) {
        x = point.x;
        y = point.y;
        return *this;
    }

    vec2d operator+(const vec2d& v) const {
        return vec2d(x + v.x, y + v.y);
    }

    vec2d operator-(const vec2d& v) const {
        return vec2d(x - v.x, y - v.y);
    }

    vec2d mul(const vec2d& vec) const {
        return vec2d(x * vec.x, y * vec.y);
    }

    vec2d operator*(const float& fl) const {
        return vec2d(x * fl, y * fl);
    }
	
    vec2d operator/(const vec2d& vec) const {
        return vec2d(x / vec.x, y / vec.y);
    }

    vec2d& operator/=(const float& fl) {
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

    float dot(const vec2d& vec) const {
        return x * vec.x + y * vec.y;
    }

    float dist(const vec2d& vec) const {
        vec2d delta;

        delta.x = x - vec.x;
        delta.y = y - vec.y;

        return delta.length();
    }

    void normalize() {
        *this /= length();
    }

    vec2d lerp(const vec2d& to, const float t) {
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

inline void lua_vec2d(lua_State* l) {
    /*luabridge::getGlobalNamespace(l)
    .beginClass<vec2d>(xs("vec2d"))
        .addConstructor<void(*)(const float&, const float&)>()
        .addData(xs("x"), &vec2d::x)
        .addData(xs("y"), &vec2d::y)
        .addFunction(xs("__add"), &vec2d::operator+)
        .addFunction(xs("__sub"), &vec2d::operator-)
        .addFunction(xs("__mul"), &vec2d::mul)
        .addFunction(xs("__div"), &vec2d::operator/)
        .addFunction(xs("__len"), &vec2d::length)
        .addFunction(xs("length"), &vec2d::length)
        .addFunction(xs("length_sqr"), &vec2d::length_sqr)
        .addFunction(xs("dot"), &vec2d::dot)
        .addFunction(xs("dist"), &vec2d::dist)
        .addFunction(xs("normalize"), &vec2d::normalize)
        .addFunction(xs("lerp"), &vec2d::lerp)
        .addFunction(xs("empty"), &vec2d::empty)
        .addFunction(xs("valid"), &vec2d::valid)
        .addFunction(xs("zero"), &vec2d::zero)
	.endClass();*/
}
