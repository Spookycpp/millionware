#pragma once

#include <algorithm>
#include <cmath>

using matrix3x4_t = typename float[3][4];
using matrix4x4_t = typename float[4][4];

struct point_t
{
	float x, y;

	constexpr point_t() : x(0.0f), y(0.0f)
	{
	}

	constexpr point_t(float value) : x(value), y(value)
	{
	}

	constexpr point_t(float x, float y) : x(x), y(y)
	{
	}
};

struct vector_t
{
	float x, y, z;

	constexpr vector_t() : x(0.0f), y(0.0f), z(0.0f)
	{
	}

	constexpr vector_t(float value) : x(value), y(value), z(value)
	{
	}

	constexpr vector_t(float x, float y, float z) : x(x), y(y), z(z)
	{
	}

	inline float length() const
	{
		return std::sqrtf(x * x + y * y + z * z);
	}

	inline float length_square() const
	{
		return x * x + y * y + z * z;
	}

	inline float length_2d() const
	{
		return std::sqrtf(x * x + y * y);
	}

	inline float length_2d_square() const
	{
		return x * x + y * y;
	}

	inline float dot_product(const vector_t &other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	inline bool empty() const 
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	inline vector_t clamp() {
		for (size_t axis = 0; axis < 2; axis++) {
			auto& cur_axis = *(float*)((uintptr_t)this + axis);

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

	inline float dist(const vector_t& vec) const {
		vector_t delta;

		delta.x = x - vec.x;
		delta.y = y - vec.y;
		delta.z = z - vec.z;

		return delta.length();
	}

	inline float dot(const vector_t& vec) const {
		return x * vec.x + y * vec.y + z * vec.z;
	}

	inline float length_sqr() const {
		return x * x + y * y + z * z;
	}
};

static inline point_t operator+(const point_t &lhs, float rhs) { return { lhs.x + rhs, lhs.y + rhs }; }
static inline point_t operator-(const point_t &lhs, float rhs) { return { lhs.x - rhs, lhs.y - rhs }; }
static inline point_t operator*(const point_t &lhs, float rhs) { return { lhs.x * rhs, lhs.y * rhs }; }
static inline point_t operator/(const point_t &lhs, float rhs) { return { lhs.x / rhs, lhs.y / rhs }; }

static inline point_t operator+(const point_t &lhs, const point_t &rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y }; }
static inline point_t operator-(const point_t &lhs, const point_t &rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y }; }
static inline point_t operator*(const point_t &lhs, const point_t &rhs) { return { lhs.x * rhs.x, lhs.y * rhs.y }; }
static inline point_t operator/(const point_t &lhs, const point_t &rhs) { return { lhs.x / rhs.x, lhs.y / rhs.y }; }

static inline point_t &operator+=(point_t &lhs, float rhs) { lhs.x += rhs; lhs.y += rhs; return lhs; }
static inline point_t &operator-=(point_t &lhs, float rhs) { lhs.x -= rhs; lhs.y -= rhs; return lhs; }
static inline point_t &operator*=(point_t &lhs, float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline point_t &operator/=(point_t &lhs, float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }

static inline point_t &operator+=(point_t &lhs, const point_t &rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline point_t &operator-=(point_t &lhs, const point_t &rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline point_t &operator*=(point_t &lhs, const point_t &rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline point_t &operator/=(point_t &lhs, const point_t &rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }

static inline vector_t operator+(const vector_t &lhs, float rhs) { return { lhs.x + rhs, lhs.y + rhs, lhs.z + rhs }; }
static inline vector_t operator-(const vector_t &lhs, float rhs) { return { lhs.x - rhs, lhs.y - rhs, lhs.z - rhs }; }
static inline vector_t operator*(const vector_t &lhs, float rhs) { return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs }; }
static inline vector_t operator/(const vector_t &lhs, float rhs) { return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs }; }

static inline vector_t operator+(const vector_t &lhs, const point_t &rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z }; }
static inline vector_t operator-(const vector_t &lhs, const point_t &rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z }; }
static inline vector_t operator*(const vector_t &lhs, const point_t &rhs) { return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z }; }
static inline vector_t operator/(const vector_t &lhs, const point_t &rhs) { return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z }; }

static inline vector_t operator+(const vector_t &lhs, const vector_t &rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z }; }
static inline vector_t operator-(const vector_t &lhs, const vector_t &rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z }; }
static inline vector_t operator*(const vector_t &lhs, const vector_t &rhs) { return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z }; }
static inline vector_t operator/(const vector_t &lhs, const vector_t &rhs) { return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z }; }

static inline vector_t &operator+=(vector_t &lhs, float rhs) { lhs.x += rhs; lhs.y += rhs; lhs.z += rhs; return lhs; }
static inline vector_t &operator-=(vector_t &lhs, float rhs) { lhs.x -= rhs; lhs.y -= rhs; lhs.z -= rhs; return lhs; }
static inline vector_t &operator*=(vector_t &lhs, float rhs) { lhs.x *= rhs; lhs.y *= rhs; lhs.z *= rhs; return lhs; }
static inline vector_t &operator/=(vector_t &lhs, float rhs) { lhs.x /= rhs; lhs.y /= rhs; lhs.z /= rhs; return lhs; }

static inline vector_t &operator+=(vector_t &lhs, const point_t &rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline vector_t &operator-=(vector_t &lhs, const point_t &rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline vector_t &operator*=(vector_t &lhs, const point_t &rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline vector_t &operator/=(vector_t &lhs, const point_t &rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }

static inline vector_t &operator+=(vector_t &lhs, const vector_t &rhs) { lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; return lhs; }
static inline vector_t &operator-=(vector_t &lhs, const vector_t &rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z; return lhs; }
static inline vector_t &operator*=(vector_t &lhs, const vector_t &rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; lhs.z *= rhs.z; return lhs; }
static inline vector_t &operator/=(vector_t &lhs, const vector_t &rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; lhs.z /= rhs.z; return lhs; }
