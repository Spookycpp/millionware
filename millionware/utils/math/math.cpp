#include <cmath>

#include "math.hpp"

vector3_t math::calculate_angle(const vector3_t& source, const vector3_t& target) {
	const auto delta = source - target;
	const auto length_2d = delta.length_2d();

	const auto x = std::atan(delta.z / delta.length_2d()) * (180.0f / PI_F);
	const auto y = std::atan(delta.y / delta.x) * (180.0f / PI_F);

	return vector3_t(x, delta.x >= 0.0f ? y + 180.0f : y, 0.0f);
}

vector3_t math::vector_angles(const vector3_t& start, const vector3_t& end)
{
	const vector3_t delta = end - start;

	const float magnitude = std::sqrt(delta.x * delta.x + delta.y * delta.y);
	const float pitch = std::atan2(-delta.z, magnitude) * RAD_PI;
	const float yaw = std::atan2(delta.y, delta.x) * RAD_PI;

	return vector3_t(pitch, yaw, 0.0f);
}

vector3_t math::transform_vector(const vector3_t& in, const matrix3x4_t& matrix) {
	return vector3_t(
		in.dot_product(vector3_t(matrix.data[0][0], matrix.data[0][1], matrix.data[0][2])) + matrix.data[0][3],
		in.dot_product(vector3_t(matrix.data[1][0], matrix.data[1][1], matrix.data[1][2])) + matrix.data[1][3],
		in.dot_product(vector3_t(matrix.data[2][0], matrix.data[2][1], matrix.data[2][2])) + matrix.data[2][3]
	);
}

bool math::normalize_angles(vector3_t& angles) {
	if (std::isfinite(angles.x) && std::isfinite(angles.y) && std::isfinite(angles.z))
	{
		angles.x = std::remainder(angles.x, 360.0f);
		angles.y = std::remainder(angles.y, 360.0f);

		return true;
	}

	return false;
}

void math::sin_cos(float radian, float* sin, float* cos) {
	*sin = std::sin(radian);
	*cos = std::cos(radian);
}

void math::angle_to_vector(const vector3_t& angles, vector3_t& forward) {
	float sp, sy, cp, cy;

	sin_cos(deg_to_rad(angles.y), &sy, &cy);
	sin_cos(deg_to_rad(angles.x), &sp, &cp);

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

float math::deg_to_rad(float degrees) {
	return degrees * PI_180_F;
}

double math::deg_to_rad(double degrees) {
	return degrees * PI_180_D;
}

float math::rad_to_deg(float radians) {
	return radians * _180_PI_F;
}

double math::rad_to_deg(double radians) {
	return radians * _180_PI_D;
}
