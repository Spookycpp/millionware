#pragma once

#include "../../sdk/vector.hpp"
#include "../../sdk/matrix.hpp"

namespace math
{
	constexpr float pi{ 3.14159265358979323846f };
	constexpr float pi_2{ pi * 2.0f };
	constexpr float pi_1divpi{ 1.0f / pi };
	constexpr float pi_1div2pi{ 1.0f / pi_2 };
	constexpr float pi_div2{ pi / 2.0f };
	constexpr float pi_div4{ pi / 4.0f };
	constexpr float rad_pi{ 57.295779513082f };
	constexpr long double pi_rad{ 0.01745329251f };

	constexpr float deg_to_rad(const float val) {
		return val * (pi / 180.0f);
	}

	constexpr float rad_to_deg(const float val) {
		return val * (180.0f / pi);
	}

	template< typename type = float >
	type min(const type& a, const type& b)
	{
		static_assert(std::is_arithmetic< type >::value, "math::min only supports integral types.");
		return (type)_mm_cvtss_f32(_mm_min_ss(_mm_set_ss(float(a)), _mm_set_ss(float(b))));
	}

	template< typename type = float >
	type max(const type& a, const type& b)
	{
		static_assert(std::is_arithmetic< type >::value, "math::max only supports integral types.");

		return (type)_mm_cvtss_f32(_mm_max_ss(_mm_set_ss(float(a)), _mm_set_ss(float(b)))
		);
	}

	template< typename type = float >
	void clamp(type& val, const type& min, const type& max)
	{
		static_assert(std::is_arithmetic< type >::value, "math::clamp only supports integral types.");
		val = static_cast<type>(_mm_cvtss_f32(_mm_min_ss(_mm_max_ss(_mm_set_ss(float(val)), _mm_set_ss(float(min))), _mm_set_ss(float(max)))));
	}

	template< typename type = float >
	type sqrt(const type& val)
	{
		static_assert(std::is_arithmetic< type >::value, "math::sqrt only supports integral types.");
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(float(val))));
	}

	bool clamp_angles(vector3_t& angles);
	bool normalize_angles(vector3_t& angles);
	float normalize_angle(float angle);
	void angle_matrix(const vector3_t& angles, matrix3x4_t& matrix);
	vector3_t to_angles(const vector3_t& vec);
	void sin_cos(float radian, float* sin, float* cos);
	void angle_to_vector(const vector3_t& angles, vector3_t& forward);
	vector3_t angle_to_vector(const vector3_t& angles);
	void matrix_position(const matrix3x4_t& matrix, vector3_t& out);
	void angle_to_vectors(const vector3_t& angles, vector3_t* forward, vector3_t* right, vector3_t* up);
	vector3_t vector_angles(const vector3_t& start, const vector3_t& end);
	vector3_t vector_to_angles(const vector3_t& start, const vector3_t& end);
	vector3_t vector_to_angles(const vector3_t& v);
	vector3_t vector_transform(const vector3_t& in, const matrix3x4_t& matrix);
	float approach_angle(float to, float from, float speed);
	vector3_t get_matrix_position(const matrix3x4_t& src);
	void set_matrix_position(vector3_t pos, matrix3x4_t& matrix);
	vector3_t dist_segment_to_segment(vector3_t s1, vector3_t s2, vector3_t k1, vector3_t k2);

	__forceinline vector3_t calc_angle(const vector3_t v1, const vector3_t v2)
	{
		const vector3_t delta = v1 - v2;

		if (delta.z == 0.0f && delta.length() == 0.0f) {
			return {};
		}

		if (delta.y == 0.0f && delta.x == 0.0f) {
			return {};
		}

		vector3_t angles = {
			std::asin(delta.z / delta.length()) * rad_pi,
			std::atan(delta.y / delta.x) * rad_pi,
			0.0f
		};

		if (delta.x >= 0.0f) {
			angles.y += 180.0f;
		}

		return angles;
	}

	__forceinline void vector_transform(const vector3_t& in, const matrix3x4_t& matrix, vector3_t& out)
	{
		out.x = in.dot(matrix.m_mat_val[0]) + matrix.m_mat_val[0][3];
		out.y = in.dot(matrix.m_mat_val[1]) + matrix.m_mat_val[1][3];
		out.z = in.dot(matrix.m_mat_val[2]) + matrix.m_mat_val[2][3];
	}

	__forceinline float get_fov(const vector3_t& view_angs, const vector3_t& aim_angles)
	{
		vector3_t ang, aim;

		angle_to_vector(view_angs, aim);
		angle_to_vector(aim_angles, ang);

		return rad_to_deg(std::acos(aim.dot(ang) / aim.length_sqr()));
	}

	template < typename type = float >
	type ease_in(const type& t1, const type& t2, float progress, const float exp = 2.0f)
	{
		progress = std::pow(progress, exp);
		return t1 + (t2 - t1) * progress;
	}

	template < typename type = float >
	type ease_out(const type& t1, const type& t2, float progress)
	{
		progress = progress * (2.0f - progress);
		return t1 + (t2 - t1) * progress;
	}

	template < typename type = float >
	type ease_inout(const type& t1, const type& t2, float progress)
	{
		progress = progress < 0.5f ? 2.0f * progress * progress : -1.0f + (4.0f - 2.0f * progress) * progress;
		return t1 + (t2 - t1) * progress;
	}

	template < typename t > __forceinline
		t lerp(const t& t1, const t& t2, float progress) {
		return t1 + (t2 - t1) * progress;
	}

	template <typename T>
	constexpr T map_range(T value, T input_min, T input_max, T output_min, T output_max) {
		static_assert(std::is_arithmetic_v<T>, "type isn't arithmetic");

		const auto slope = (output_max - output_min) / (input_max - input_min);
		const auto output = output_min + slope * (value - input_min);

		return output;
	}
}
