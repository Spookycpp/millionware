#include <cmath>

#include "math.hpp"

vector3_t math::calculate_angle(const vector3_t& source, const vector3_t& target) {
	const auto delta = source - target;
	const auto length_2d = delta.length_2d();

	const auto x = std::atan(delta.z / delta.length_2d()) * (180.0f / PI_F);
	const auto y = std::atan(delta.y / delta.x) * (180.0f / PI_F);

	return vector3_t(x, delta.x >= 0.0f ? y + 180.0f : y, 0.0f);
}

vector3_t math::vector_angles(const vector3_t& vector) {
	if (vector.y <= 0.0001f && vector.z <= 0.0001f)
		return vector3_t(vector.z > 0.0f ? 270.0f : 90.0f, 0.0f, 0.0f);

	auto out_x = std::atan2(-vector.z, vector.length_2d()) * 180.0f / PI_F;
	auto out_y = std::atan2(vector.y, vector.x) * 180.0f / PI_F;

	if (out_x < 0.0f)
		out_x += 360;

	if (out_y < 0.0f)
		out_y += 360;

	return vector3_t(out_x, out_y, 0.0f);
}

vector3_t math::transform_vector(const vector3_t& in, const matrix3x4_t& matrix) {
	return vector3_t(
		in.dot_product(vector3_t(matrix.data[0][0], matrix.data[0][1], matrix.data[0][2])) + matrix.data[0][3],
		in.dot_product(vector3_t(matrix.data[1][0], matrix.data[1][1], matrix.data[1][2])) + matrix.data[1][3],
		in.dot_product(vector3_t(matrix.data[2][0], matrix.data[2][1], matrix.data[2][2])) + matrix.data[2][3]
	);
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
