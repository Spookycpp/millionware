#pragma once

#include "../sdk/matrix.hpp"
#include "../sdk/vector.hpp"

namespace math {

	constexpr auto PI_F = 3.14159265358979323846f;
	constexpr auto PI_D = 3.141592653589793238462643383279502884;

	constexpr auto PI_180_F = PI_F / 180.0f;
	constexpr auto PI_180_D = PI_D / 180.0;

	constexpr auto _180_PI_F = 180.0f / PI_F;
	constexpr auto _180_PI_D = 180.0f / PI_D;

	vector3_t calculate_angle(const vector3_t& source, const vector3_t& target);
	vector3_t vector_angles(const vector3_t& vector);
	vector3_t transform_vector(const vector3_t& in, const matrix3x4_t& matrix);

	float deg_to_rad(float degrees);
	double deg_to_rad(double degrees);

	float rad_to_deg(float degrees);
	double rad_to_deg(double degrees);

	template <typename T>
	constexpr T map_range(T value, T input_min, T input_max, T output_min, T output_max) {
		static_assert(std::is_arithmetic_v<T>, "type isn't arithmetic");

		const auto slope = (output_max - output_min) / (input_max - input_min);
		const auto output = output_min + slope * (value - input_min);

		return output;
	}

}
