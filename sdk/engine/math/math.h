#pragma once

#include "../../source engine/vector.h"

using matrix3x4_t = typename float[3][4];
using matrix4x4_t = typename float[4][4];

namespace math
{
	constexpr static float pi = 3.14159265358979323846f;
	constexpr static float pi_2 = pi * 2.0f;
	constexpr static float pi_over_2 = pi / 2.0f;
	constexpr static float pi_over_4 = pi / 4.0f;
	constexpr static float rad_pi = 57.295779513082f;
	constexpr static float pi_rad = 0.01745329251f;

	constexpr float deg_to_rad(float val)
	{
		return val * (pi / 180.0f);
	}

	constexpr float rad_to_deg(float val)
	{
		return val * (180.0f / pi);
	}

	void angle_to_vectors(const vector_t& angles, vector_t* forward, vector_t* right, vector_t* up);
	void sin_cos(float radian, float* sin, float* cos);
	void set_matrix_position(vector_t pos, matrix3x4_t& matrix);

	bool world_to_screen(const vector_t &world_pos, point_t &screen_pos);
	bool normalize_angles(vector_t &angle);
	bool clamp_angles(vector_t &angle);

	float get_fov(const vector_t &view_angle, const vector_t &aim_angle);

	vector_t get_forward_vector(const vector_t &angle);
	vector_t get_right_vector(const vector_t &angle);
	vector_t get_up_vector(const vector_t &angle);

	vector_t angle_to_vector(const vector_t& angles);
	vector_t dist_segment_to_segment(vector_t s1, vector_t s2, vector_t k1, vector_t k2);
	vector_t vector_angles(const vector_t &source, const vector_t &target);
	vector_t vector_transform(const vector_t &vector, const matrix3x4_t &transformation_matrix);
	vector_t get_matrix_position(const matrix3x4_t& src);

	template < typename type = float >
	type ease_in(const type& t1, const type& t2, float progress, const float exp = 2.0f) {
		progress = std::pow(progress, exp);
		return t1 + (t2 - t1) * progress;
	}

	template < typename type = float >
	type ease_out(const type& t1, const type& t2, float progress) {
		progress = progress * (2.0f - progress);
		return t1 + (t2 - t1) * progress;
	}

	template < typename type = float >
	type ease_inout(const type& t1, const type& t2, float progress) {
		progress = progress < 0.5f ? 2.0f * progress * progress : -1.0f + (4.0f - 2.0f * progress) * progress;
		return t1 + (t2 - t1) * progress;
	}

	template < typename t > __forceinline
		t lerp(const t& t1, const t& t2, float progress) {
		return t1 + (t2 - t1) * progress;
	}
}
