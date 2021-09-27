#pragma once

#include <algorithm>
#include <cmath>

struct point_t {
    float x, y;

    constexpr point_t() : x(0.0f), y(0.0f) {
    }

    constexpr point_t(float value) : x(value), y(value) {
    }

    constexpr point_t(float x, float y) : x(x), y(y) {
    }
};

struct vector_t {
    float x, y, z;

    constexpr vector_t() : x(0.0f), y(0.0f), z(0.0f) {
    }

    constexpr vector_t(float value) : x(value), y(value), z(value) {
    }

    constexpr vector_t(float x, float y, float z) : x(x), y(y), z(z) {
    }

    inline float length() const {
        return std::sqrtf(x * x + y * y + z * z);
    }

    inline float length_square() const {
        return x * x + y * y + z * z;
    }

    inline float length_2d() const {
        return std::sqrtf(x * x + y * y);
    }

    inline float length_2d_square() const {
        return x * x + y * y;
    }

    inline float dot_product(const vector_t &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    inline bool empty() const {
        return x == 0.0f && y == 0.0f && z == 0.0f;
    }

    inline vector_t clamp() {
        for (size_t axis = 0; axis < 2; axis++) {
            auto &cur_axis = *(float *) ((uintptr_t) this + axis);

            if (!std::isfinite(cur_axis)) {
                cur_axis = 0.0f;
            }
        }

        x = std::clamp(x, -89.0f, 89.0f);
        y = std::clamp(std::remainder(y, 360.0f), -180.0f, 180.0f);
        z = 0.0f;

        return *this;
    }

    inline void init(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }

    inline float dist(const vector_t &vec) const {
        vector_t delta;

        delta.x = x - vec.x;
        delta.y = y - vec.y;
        delta.z = z - vec.z;

        return delta.length();
    }

    inline float dist_2d(const vector_t &vec) {
        vector_t delta;

        delta.x = x - vec.x;
        delta.y = y - vec.y;
        delta.z = z - vec.z;

        return delta.length_2d();
    }

    __forceinline float dot(const float *fl) const {
        const vector_t &a = *this;
        return a.x * fl[0] + a.y * fl[1] + a.z * fl[2];
    }

    inline float dot(const vector_t &vec) const {
        return x * vec.x + y * vec.y + z * vec.z;
    }

    __forceinline float &operator[](int32_t i) {
        return reinterpret_cast<float *>(this)[i];
    }

    __forceinline float operator[](int32_t i) const {
        return reinterpret_cast<float *>((vector_t *) this)[i];
    }

    inline float length_sqr() const {
        return x * x + y * y + z * z;
    }

    inline float normalize_in_place() {
        vector_t &v = *this;

        const float radius = 1.0f / (this->length() + FLT_EPSILON);

        v.x *= radius;
        v.y *= radius;
        v.z *= radius;

        return 1;
    }

    inline void normalize() {
        auto floorf = [&](float x) { return float((int) x); };

        auto roundf = [&](float x) {
            float t;

            // if (!isfinite(x))
            //    return x;

            if (x >= 0.0) {
                t = floorf(x);
                if (t - x <= -0.5)
                    t += 1.0;
                return t;
            } else {
                t = floorf(-x);
                if (t + x <= -0.5)
                    t += 1.0;
                return -t;
            }
        };

        auto absf = [](float x) { return x > 0.f ? x : -x; };

        auto x_rev = this->x / 360.f;
        if (this->x > 180.f || this->x < -180.f) {
            x_rev = absf(x_rev);
            x_rev = roundf(x_rev);

            if (this->x < 0.f)
                this->x = (this->x + 360.f * x_rev);

            else
                this->x = (this->x - 360.f * x_rev);
        }

        auto y_rev = this->y / 360.f;
        if (this->y > 180.f || this->y < -180.f) {
            y_rev = absf(y_rev);
            y_rev = roundf(y_rev);

            if (this->y < 0.f)
                this->y = (this->y + 360.f * y_rev);

            else
                this->y = (this->y - 360.f * y_rev);
        }

        auto z_rev = this->z / 360.f;
        if (this->z > 180.f || this->z < -180.f) {
            z_rev = absf(z_rev);
            z_rev = roundf(z_rev);

            if (this->z < 0.f)
                this->z = (this->z + 360.f * z_rev);

            else
                this->z = (this->z - 360.f * z_rev);
        }
    }
};

static inline point_t operator+(const point_t &lhs, float rhs) {
    return {lhs.x + rhs, lhs.y + rhs};
}
static inline point_t operator-(const point_t &lhs, float rhs) {
    return {lhs.x - rhs, lhs.y - rhs};
}
static inline point_t operator*(const point_t &lhs, float rhs) {
    return {lhs.x * rhs, lhs.y * rhs};
}
static inline point_t operator/(const point_t &lhs, float rhs) {
    return {lhs.x / rhs, lhs.y / rhs};
}

static inline point_t operator+(const point_t &lhs, const point_t &rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}
static inline point_t operator-(const point_t &lhs, const point_t &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}
static inline point_t operator*(const point_t &lhs, const point_t &rhs) {
    return {lhs.x * rhs.x, lhs.y * rhs.y};
}
static inline point_t operator/(const point_t &lhs, const point_t &rhs) {
    return {lhs.x / rhs.x, lhs.y / rhs.y};
}

static inline point_t &operator+=(point_t &lhs, float rhs) {
    lhs.x += rhs;
    lhs.y += rhs;
    return lhs;
}
static inline point_t &operator-=(point_t &lhs, float rhs) {
    lhs.x -= rhs;
    lhs.y -= rhs;
    return lhs;
}
static inline point_t &operator*=(point_t &lhs, float rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}
static inline point_t &operator/=(point_t &lhs, float rhs) {
    lhs.x /= rhs;
    lhs.y /= rhs;
    return lhs;
}

static inline point_t &operator+=(point_t &lhs, const point_t &rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}
static inline point_t &operator-=(point_t &lhs, const point_t &rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}
static inline point_t &operator*=(point_t &lhs, const point_t &rhs) {
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    return lhs;
}
static inline point_t &operator/=(point_t &lhs, const point_t &rhs) {
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    return lhs;
}

static inline vector_t operator+(const vector_t &lhs, float rhs) {
    return {lhs.x + rhs, lhs.y + rhs, lhs.z + rhs};
}
static inline vector_t operator-(const vector_t &lhs, float rhs) {
    return {lhs.x - rhs, lhs.y - rhs, lhs.z - rhs};
}
static inline vector_t operator*(const vector_t &lhs, float rhs) {
    return {lhs.x * rhs, lhs.y * rhs, lhs.z * rhs};
}
static inline vector_t operator/(const vector_t &lhs, float rhs) {
    return {lhs.x / rhs, lhs.y / rhs, lhs.z / rhs};
}

static inline vector_t operator+(const vector_t &lhs, const point_t &rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z};
}
static inline vector_t operator-(const vector_t &lhs, const point_t &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z};
}
static inline vector_t operator*(const vector_t &lhs, const point_t &rhs) {
    return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z};
}
static inline vector_t operator/(const vector_t &lhs, const point_t &rhs) {
    return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z};
}

static inline vector_t operator+(const vector_t &lhs, const vector_t &rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}
static inline vector_t operator-(const vector_t &lhs, const vector_t &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}
static inline vector_t operator*(const vector_t &lhs, const vector_t &rhs) {
    return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
}
static inline vector_t operator/(const vector_t &lhs, const vector_t &rhs) {
    return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z};
}

static inline vector_t &operator+=(vector_t &lhs, float rhs) {
    lhs.x += rhs;
    lhs.y += rhs;
    lhs.z += rhs;
    return lhs;
}
static inline vector_t &operator-=(vector_t &lhs, float rhs) {
    lhs.x -= rhs;
    lhs.y -= rhs;
    lhs.z -= rhs;
    return lhs;
}
static inline vector_t &operator*=(vector_t &lhs, float rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
    return lhs;
}
static inline vector_t &operator/=(vector_t &lhs, float rhs) {
    lhs.x /= rhs;
    lhs.y /= rhs;
    lhs.z /= rhs;
    return lhs;
}

static inline vector_t &operator+=(vector_t &lhs, const point_t &rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}
static inline vector_t &operator-=(vector_t &lhs, const point_t &rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}
static inline vector_t &operator*=(vector_t &lhs, const point_t &rhs) {
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    return lhs;
}
static inline vector_t &operator/=(vector_t &lhs, const point_t &rhs) {
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    return lhs;
}

static inline vector_t &operator+=(vector_t &lhs, const vector_t &rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}
static inline vector_t &operator-=(vector_t &lhs, const vector_t &rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}
static inline vector_t &operator*=(vector_t &lhs, const vector_t &rhs) {
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    lhs.z *= rhs.z;
    return lhs;
}
static inline vector_t &operator/=(vector_t &lhs, const vector_t &rhs) {
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    lhs.z /= rhs.z;
    return lhs;
}