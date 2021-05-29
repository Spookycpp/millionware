#pragma once

class vec3d {
  public:
    float x, y, z;

    vec3d() : x{0.0f}, y{0.0f}, z{0.0f} {}
    vec3d(const float x, const float y, const float z) : x{x}, y{y}, z{z} {}

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
        return sqrt(x * x + y * y + z * z);
    }

    float length_sqr() const {
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

inline void lua_vec3d(lua_State *l) {
    luabridge::getGlobalNamespace(l)
        .beginClass<vec3d>("vec3d")
        .addConstructor<void (*)(const float &, const float &, const float &)>()
        .addData("x", &vec3d::x)
        .addData("y", &vec3d::y)
        .addData("z", &vec3d::z)
        .addFunction("__add", &vec3d::operator+)
        .addFunction("__sub", &vec3d::operator-)
        .addFunction("__mul", &vec3d::mul)
        .addFunction("__div", &vec3d::operator/)
        .addFunction("__len", &vec3d::length)
        .addFunction("length", &vec3d::length)
        .addFunction("length_sqr", &vec3d::length_sqr)
        .addFunction("dot", &vec3d::dot)
        .addFunction("dist", &vec3d::dist)
        .addFunction("normalize", &vec3d::normalize)
        .addFunction("lerp", &vec3d::lerp)
        .addFunction("empty", &vec3d::empty)
        .addFunction("valid", &vec3d::valid)
        .addFunction("zero", &vec3d::zero)
        .endClass();
}
