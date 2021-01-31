#pragma once

#include <algorithm>
#include <cstdint>
#include <cmath>

struct point_t {
	int x, y;

	constexpr point_t(int x, int y)
		: x(x), y(y) {}

	constexpr point_t(int scalar)
		: x(scalar), y(scalar) {}

	constexpr point_t()
		: x(0), y(0) {}

	point_t operator +(const point_t rhs) const {
		return point_t(x + rhs.x, y + rhs.y);
	}

	point_t operator -(const point_t rhs) const {
		return point_t(x - rhs.x, y - rhs.y);
	}

	point_t operator *(const point_t rhs) const {
		return point_t(x * rhs.x, y * rhs.y);
	}

	point_t operator /(const point_t rhs) const {
		return point_t(x / rhs.x, y / rhs.y);
	}

	point_t operator +(int rhs) const {
		return point_t(x + rhs, y + rhs);
	}

	point_t operator -(int rhs) const {
		return point_t(x - rhs, y - rhs);
	}

	point_t operator *(int rhs) const {
		return point_t(x * rhs, y * rhs);
	}

	point_t operator /(int rhs) const {
		return point_t(x / rhs, y / rhs);
	}

	point_t& operator +=(const point_t rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	point_t& operator -=(const point_t rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	point_t& operator *=(const point_t rhs) {
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}

	point_t& operator /=(const point_t rhs) {
		x /= rhs.x;
		y /= rhs.y;
		return *this;
	}

	point_t& operator +=(int rhs) {
		x += rhs;
		y += rhs;
		return *this;
	}

	point_t& operator -=(int rhs) {
		x -= rhs;
		y -= rhs;
		return *this;
	}

	point_t& operator *=(int rhs) {
		x *= rhs;
		y *= rhs;
		return *this;
	}

	point_t& operator /=(int rhs) {
		x /= rhs;
		y /= rhs;
		return *this;
	}
};

struct vector2_t {
	float x, y;

	constexpr vector2_t(float x, float y)
		: x(x), y(y) {}

	constexpr vector2_t(float scalar)
		: x(scalar), y(scalar) {}

	constexpr vector2_t()
		: x(0.0f), y(0.0f) {}

	vector2_t operator +(const vector2_t rhs) const {
		return vector2_t(x + rhs.x, y + rhs.y);
	}

	vector2_t operator -(const vector2_t rhs) const {
		return vector2_t(x - rhs.x, y - rhs.y);
	}

	vector2_t operator *(const vector2_t rhs) const {
		return vector2_t(x * rhs.x, y * rhs.y);
	}

	vector2_t operator /(const vector2_t rhs) const {
		return vector2_t(x / rhs.x, y / rhs.y);
	}

	vector2_t operator +(float rhs) const {
		return vector2_t(x + rhs, y + rhs);
	}

	vector2_t operator -(float rhs) const {
		return vector2_t(x - rhs, y - rhs);
	}

	vector2_t operator *(float rhs) const {
		return vector2_t(x * rhs, y * rhs);
	}

	vector2_t operator /(float rhs) const {
		return vector2_t(x / rhs, y / rhs);
	}

	vector2_t& operator +=(const vector2_t rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	vector2_t& operator -=(const vector2_t rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	vector2_t& operator *=(const vector2_t rhs) {
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}

	vector2_t& operator /=(const vector2_t rhs) {
		x /= rhs.x;
		y /= rhs.y;
		return *this;
	}

	vector2_t& operator +=(float rhs) {
		x += rhs;
		y += rhs;
		return *this;
	}

	vector2_t& operator -=(float rhs) {
		x -= rhs;
		y -= rhs;
		return *this;
	}

	vector2_t& operator *=(float rhs) {
		x *= rhs;
		y *= rhs;
		return *this;
	}

	vector2_t& operator /=(float rhs) {
		x /= rhs;
		y /= rhs;
		return *this;
	}

	float length() const {
		return std::sqrtf(x * x + y * y);
	}
};

struct vector3_t {
	float x, y, z;

	constexpr bool empty() const {
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	constexpr vector3_t(float x, float y, float z)
		: x(x), y(y), z(z) {}

	constexpr vector3_t(const vector2_t& vec)
		: x(vec.x), y(vec.y), z(0.0f) {}

	constexpr vector3_t(const vector2_t& vec, float z)
		: x(vec.x), y(vec.y), z(z) {}

	constexpr vector3_t(float scalar)
		: x(scalar), y(scalar), z(scalar) {}

	constexpr vector3_t()
		: x(0.0f), y(0.0f), z(0.0f) {}

	vector3_t operator +(const vector3_t& rhs) const {
		return vector3_t(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	vector3_t operator -(const vector3_t& rhs) const {
		return vector3_t(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	vector3_t operator *(const vector3_t& rhs) const {
		return vector3_t(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	vector3_t operator /(const vector3_t& rhs) const {
		return vector3_t(x / rhs.x, y / rhs.y, z / rhs.z);
	}

	vector3_t operator +(float rhs) const {
		return vector3_t(x + rhs, y + rhs, z + rhs);
	}

	vector3_t operator -(float rhs) const {
		return vector3_t(x - rhs, y - rhs, z - rhs);
	}

	vector3_t operator *(float rhs) const {
		return vector3_t(x * rhs, y * rhs, z * rhs);
	}

	vector3_t operator /(float rhs) const {
		return vector3_t(x / rhs, y / rhs, z / rhs);
	}

	vector3_t& operator +=(const vector3_t& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	vector3_t& operator -=(const vector3_t& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	vector3_t& operator *=(const vector3_t& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	vector3_t& operator /=(const vector3_t& rhs) {
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		return *this;
	}

	vector3_t& operator +=(float rhs) {
		x += rhs;
		y += rhs;
		z += rhs;
		return *this;
	}

	vector3_t& operator -=(float rhs) {
		x -= rhs;
		y -= rhs;
		z -= rhs;
		return *this;
	}

	vector3_t& operator *=(float rhs) {
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}

	vector3_t& operator /=(float rhs) {
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}

	float length() const {
		return std::sqrtf(x * x + y * y + z * z);
	}

	float length_2d() const {
		return std::sqrtf(x * x + y * y);
	}

	constexpr float dot_product(const vector3_t& rhs) const {
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	__forceinline float dot(const vector3_t& vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	__forceinline float dot(const float* fl) const
	{
		const vector3_t& a = *this;
		return a.x * fl[0] + a.y * fl[1] + a.z * fl[2];
	}

	__forceinline float length_sqr() const {
		return x * x + y * y + z * z;
	}

	__forceinline float dist(const vector3_t& vec) const
	{
		vector3_t delta;

		delta.x = x - vec.x;
		delta.y = y - vec.y;
		delta.z = z - vec.z;

		return delta.length();
	}

	__forceinline vector3_t clamp()
	{
		for (size_t axis = 0; axis < 2; axis++)
		{
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

};
