#include "math.hpp"

#include <algorithm>

bool math::clamp_angles(vector3_t& angles)
{
	if (std::isfinite(angles.x) && std::isfinite(angles.y) && std::isfinite(angles.z))
	{
		angles.x = std::clamp(angles.x, -89.0f, 89.0f);
		angles.y = std::clamp(angles.y, -180.0f, 180.0f);
		angles.z = 0.0f;

		return true;
	}

	return false;
}

bool math::normalize_angles(vector3_t& angles)
{
	if (std::isfinite(angles.x) && std::isfinite(angles.y) && std::isfinite(angles.z))
	{
		angles.x = std::remainder(angles.x, 360.0f);
		angles.y = std::remainder(angles.y, 360.0f);

		return true;
	}

	return false;
}

float math::normalize_angle(const float angle)
{
	if (!std::isfinite(angle)) {
		return 0.0f;
	}

	return std::remainder(angle, 360.0f);
}

float math::strafe_opti_normalize_angle(float angle, float max) {
	while (angle > max)
		angle -= max * 2;
	while (angle < -max)
		angle += max * 2;
	return angle;
}

float math::normalized_angle(float angle)
{
	normalize_angle(angle);
	return angle;
}

float math::normalize_yaw(float angle) {
	if (!std::isfinite(angle))
		angle = 0.f;

	return std::remainderf(angle, 360.0f);
}

vector3_t math::to_angles(const vector3_t& vec)
{
	vector3_t angs;

	if (vec.x == 0.0f && vec.y == 0.0f)
	{
		angs.x = (vec.z > 0.0f) ? 270.0f : 90.0f;
		angs.y = 0.0f;
	}
	else
	{
		angs.x = std::atan2(-vec.z, vec.length_2d()) * rad_pi;

		if (angs.x < 0.0f) {
			angs.x += 360.0f;
		}

		angs.y = std::atan2(vec.y, vec.x) * rad_pi;

		if (angs.y < 0.0f) {
			angs.y += 360.0f;
		}
	}

	angs.z = 0.0f;

	return angs;
}

void math::sin_cos(const float radian, float* sin, float* cos)
{
	*sin = std::sin(radian);
	*cos = std::cos(radian);
}

void math::angle_to_vector(const vector3_t& angles, vector3_t& forward)
{
	float sp, sy, cp, cy;

	sin_cos(deg_to_rad(angles.y), &sy, &cy);
	sin_cos(deg_to_rad(angles.x), &sp, &cp);

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

vector3_t math::angle_to_vector(const vector3_t& angles)
{
	float sp, sy, cp, cy;

	sin_cos(deg_to_rad(angles.y), &sy, &cy);
	sin_cos(deg_to_rad(angles.x), &sp, &cp);

	return { cp * cy, cp * sy, -sp };
}

void math::matrix_position(const matrix3x4_t& matrix, vector3_t& out)
{
	out.x = matrix.m_mat_val[0][3];
	out.y = matrix.m_mat_val[1][3];
	out.z = matrix.m_mat_val[2][3];
}

void math::angle_to_vectors(const vector3_t& angles, vector3_t* forward, vector3_t* right, vector3_t* up)
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

vector3_t math::vector_angles(const vector3_t& start, const vector3_t& end)
{
	const vector3_t delta = end - start;

	const float magnitude = std::sqrt(delta.x * delta.x + delta.y * delta.y);
	const float pitch = std::atan2(-delta.z, magnitude) * rad_pi;
	const float yaw = std::atan2(delta.y, delta.x) * rad_pi;

	return vector3_t(pitch, yaw, 0.0f);
}

vector3_t math::vector_to_angles(const vector3_t& start, const vector3_t& end)
{
	const vector3_t delta = end - start;

	const float magnitude = std::sqrt(delta.x * delta.x + delta.y * delta.y);
	const float pitch = std::atan2(-delta.z, magnitude) * rad_pi;
	const float yaw = std::atan2(delta.y, delta.x) * rad_pi;

	return vector3_t(pitch, yaw, 0.0f);
}

vector3_t math::vector_to_angles(const vector3_t& v)
{
	const float magnitude = std::sqrt(v.x * v.x + v.y * v.y);
	const float pitch = std::atan2(-v.z, magnitude) * rad_pi;
	const float yaw = std::atan2(v.y, v.x) * rad_pi;

	return vector3_t(pitch, yaw, 0.0f);
}

void math::angle_matrix(const vector3_t& angles, matrix3x4_t& matrix)
{
	float sr, sp, sy, cr, cp, cy;

	sin_cos(deg_to_rad(angles.x), &sp, &cp);
	sin_cos(deg_to_rad(angles.y), &sy, &cy);
	sin_cos(deg_to_rad(angles.z), &sr, &cr);

	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][0] = -sp;

	const float crcy = cr * cy,
		crsy = cr * sy,
		srcy = sr * cy,
		srsy = sr * sy;

	matrix[0][1] = sp * srcy - crsy;
	matrix[1][1] = sp * srsy + crcy;
	matrix[2][1] = sr * cp;

	matrix[0][2] = (sp * crcy + srsy);
	matrix[1][2] = (sp * crsy - srcy);
	matrix[2][2] = cr * cp;

	matrix[0][3] = 0.0f;
	matrix[1][3] = 0.0f;
	matrix[2][3] = 0.0f;
}

vector3_t math::vector_transform(const vector3_t& in, const matrix3x4_t& matrix)
{
	return vector3_t(
		in.dot(matrix[0]) + matrix[0][3],
		in.dot(matrix[1]) + matrix[1][3],
		in.dot(matrix[2]) + matrix[2][3]
	);
}

float math::approach_angle(const float to, float from, const float speed)
{
	const float delta = std::remainder(to - from, 360.0f);

	if (delta > speed) {
		from += speed;
	}
	else if (delta < -speed) {
		from -= speed;
	}
	else {
		from = to;
	}

	return std::clamp(from, -180.0f, 180.0f);
}

vector3_t math::get_matrix_position(const matrix3x4_t& src) {
	return vector3_t(src[0][3], src[1][3], src[2][3]);
}

void math::set_matrix_position(vector3_t pos, matrix3x4_t &matrix )
{
	for ( size_t i = 0; i < 3; ++i ) {
		matrix [ i ][ 3 ] = ((float*)&pos) [ i ];
	}
}

vector3_t math::dist_segment_to_segment(const vector3_t s1, const vector3_t s2, const vector3_t k1, const vector3_t k2)
{
	static constexpr float SMALL_NUM = 0.00000001f;

	const vector3_t u = s2 - s1;
	const vector3_t v = k2 - k1;
	const vector3_t w = s1 - k1;

	const float a = u.dot(u);
	const float b = u.dot(v);
	const float c = v.dot(v);
	const float d = u.dot(w);
	const float e = v.dot(w);

	const float D = a * c - b * b;

	float sN, sD = D;
	float tN, tD = D;

	if (D < SMALL_NUM)
	{
		sN = 0.0f;
		sD = 1.0f;
		tN = e;
		tD = c;
	}
	else
	{
		sN = (b * e - c * d);
		tN = (a * e - b * d);
		if (sN < 0.0f)
		{
			sN = 0.0f;
			tN = e;
			tD = c;
		}
		else if (sN > sD)
		{
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0f)
	{
		tN = 0.0;

		if (-d < 0.0f) {
			sN = 0.0;
		}
		else if (-d > a) {
			sN = sD;
		}
		else
		{
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD)
	{
		tN = tD;

		if (-d + b < 0.0f) {
			sN = 0;
		}
		else if (-d + b > a) {
			sN = sD;
		}
		else
		{
			sN = -d + b;
			sD = a;
		}
	}

	const float sc = std::abs(sN) < SMALL_NUM ? 0.0f : sN / sD;
	const float tc = std::abs(tN) < SMALL_NUM ? 0.0f : tN / tD;

	return w + u * sc - v * tc;
}