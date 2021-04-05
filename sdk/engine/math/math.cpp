#include <algorithm>
#include <cmath>

#include "../../core/patterns/patterns.h"
#include "../render/render.h"
#include "math.h"

void math::angle_to_vectors(const vector_t& angles, vector_t* forward, vector_t* right, vector_t* up) 
{
	float sp, sy, sr, cp, cy, cr;

	sin_cos(deg_to_rad(angles.x), &sp, &cp);
	sin_cos(deg_to_rad(angles.y), &sy, &cy);
	sin_cos(deg_to_rad(angles.z), &sr, &cr);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;

	right->x = -1 * sr * sp * cy + -1 * cr * -sy;
	right->y = -1 * sr * sp * sy + -1 * cr * cy;
	right->z = -1 * sr * cp;

	up->x = cr * sp * cy + -sr * -sy;
	up->y = cr * sp * sy + -sr * cy;
	up->z = cr * cp;
}

void math::sin_cos(float radian, float* sin, float* cos)
{
	*sin = std::sin(radian);
	*cos = std::cos(radian);
}

void math::set_matrix_position(vector_t pos, matrix3x4_t& matrix)
{
	matrix[0][3] = pos.x;
	matrix[1][3] = pos.y;
	matrix[2][3] = pos.z;
}

bool math::world_to_screen(const vector_t &world_pos, point_t &screen_pos)
{
	const static auto &view_matrix = *(matrix4x4_t *) patterns::get_view_matrix();

	const auto x = view_matrix[0][0] * world_pos.x + view_matrix[0][1] * world_pos.y + view_matrix[0][2] * world_pos.z + view_matrix[0][3];
	const auto y = view_matrix[1][0] * world_pos.x + view_matrix[1][1] * world_pos.y + view_matrix[1][2] * world_pos.z + view_matrix[1][3];
	const auto w = view_matrix[3][0] * world_pos.x + view_matrix[3][1] * world_pos.y + view_matrix[3][2] * world_pos.z + view_matrix[3][3];

	screen_pos.x = x;
	screen_pos.y = y;

	if (w < 0.01f)
		return false;

	const auto inverse_w = 1.0f / w;

	screen_pos.x *= inverse_w;
	screen_pos.y *= inverse_w;

	const auto screen_size = render::get_screen_size();
	const auto screen_center = screen_size * 0.5f;

	screen_pos.x = screen_center.x + screen_pos.x * 0.5f * screen_size.x + 0.5f;
	screen_pos.y = screen_center.y - screen_pos.y * 0.5f * screen_size.y + 0.5f;

	return true;
}

bool math::normalize_angles(vector_t &angle)
{
	if (std::isfinite(angle.x) && std::isfinite(angle.y) && std::isfinite(angle.z))
	{
		angle.x = std::remainder(angle.x, 360.0f);
		angle.y = std::remainder(angle.y, 360.0f);

		return true;
	}

	return false;
}

bool math::clamp_angles(vector_t &angle)
{
	if (std::isfinite(angle.x) && std::isfinite(angle.y) && std::isfinite(angle.z))
	{
		angle.x = std::clamp(angle.x, -89.0f, 89.0f);
		angle.y = std::clamp(angle.y, -180.0f, 180.0f);
		angle.z = 0.0f;

		return true;
	}

	return false;
}

float math::get_fov(const vector_t &view_angle, const vector_t &aim_angle)
{
	const auto angle_forward = get_forward_vector(view_angle);
	const auto aim_forward = get_forward_vector(aim_angle);

	return rad_to_deg(std::acos(aim_forward.dot_product(angle_forward) / aim_forward.length_square()));
}

vector_t math::get_forward_vector(const vector_t &angle)
{
	const auto sp = std::sin(angle.x);
	const auto sy = std::sin(angle.y);
	const auto cp = std::cos(angle.x);
	const auto cy = std::cos(angle.y);

	return { cp * cy, cp * sy, -sp };
}

vector_t math::get_right_vector(const vector_t &angle)
{
	const auto sp = std::sin(angle.x);
	const auto sy = std::sin(angle.y);
	const auto sr = std::sin(angle.z);
	const auto cp = std::cos(angle.x);
	const auto cy = std::cos(angle.y);
	const auto cr = std::cos(angle.z);

	return
	{
		-1.0f * sr * sp * cy + -1.0f * cr * -sy,
		-1.0f * sr * sp * sy + -1.0f * cr * cy,
		-1.0f * sr * cp,
	};
}

vector_t math::get_up_vector(const vector_t &angle)
{
	const auto sp = std::sin(angle.x);
	const auto sy = std::sin(angle.y);
	const auto sr = std::sin(angle.z);
	const auto cp = std::cos(angle.x);
	const auto cy = std::cos(angle.y);
	const auto cr = std::cos(angle.z);

	return
	{
		cr * sp * cy + -sr * -sy,
		cr * sp * sy + -sr * cy,
		cr * cp
	};
}

vector_t math::angle_to_vector(const vector_t& angles)
{
	float sp, sy, cp, cy;

	sin_cos(deg_to_rad(angles.y), &sy, &cy);
	sin_cos(deg_to_rad(angles.x), &sp, &cp);

	return { cp * cy, cp * sy, -sp };
}

vector_t math::dist_segment_to_segment(vector_t s1, vector_t s2, vector_t k1, vector_t k2) {
	static constexpr float SMALL_NUM = 0.00000001f;

	const vector_t u = s2 - s1;
	const vector_t v = k2 - k1;
	const vector_t w = s1 - k1;

	const float a = u.dot(u);
	const float b = u.dot(v);
	const float c = v.dot(v);
	const float d = u.dot(w);
	const float e = v.dot(w);

	const float D = a * c - b * b;

	float sN, sD = D;
	float tN, tD = D;

	if (D < SMALL_NUM) {
		sN = 0.0f;
		sD = 1.0f;
		tN = e;
		tD = c;
	}
	else {
		sN = (b * e - c * d);
		tN = (a * e - b * d);
		if (sN < 0.0f) {
			sN = 0.0f;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0f) {
		tN = 0.0;

		if (-d < 0.0f) {
			sN = 0.0;
		}
		else if (-d > a) {
			sN = sD;
		}
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {
		tN = tD;

		if (-d + b < 0.0f) {
			sN = 0;
		}
		else if (-d + b > a) {
			sN = sD;
		}
		else {
			sN = -d + b;
			sD = a;
		}
	}

	const float sc = std::abs(sN) < SMALL_NUM ? 0.0f : sN / sD;
	const float tc = std::abs(tN) < SMALL_NUM ? 0.0f : tN / tD;

	return w + u * sc - v * tc;
}

vector_t math::vector_angles(const vector_t &source, const vector_t &target)
{
	const auto delta = target - source;
	const auto magnitude = std::sqrt(delta.x * delta.x + delta.y * delta.y);
	const auto pitch = std::atan2(-delta.z, magnitude) * rad_pi;
	const auto yaw = std::atan2(delta.y, delta.x) * rad_pi;

	return { pitch, yaw, 0.0f };
}

vector_t math::vector_transform(const vector_t &vector, const matrix3x4_t &transformation_matrix)
{
	return
	{
		vector.dot_product((const vector_t &) transformation_matrix[0]) + transformation_matrix[0][3],
		vector.dot_product((const vector_t &) transformation_matrix[1]) + transformation_matrix[1][3],
		vector.dot_product((const vector_t &) transformation_matrix[2]) + transformation_matrix[2][3]
	};
}

vector_t math::get_matrix_position(const matrix3x4_t& src) {
	return vector_t(src[0][3], src[1][3], src[2][3]);
}
