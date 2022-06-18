#pragma once

#include "../../source engine/matrix.h"
#include "../../source engine/vector.h"
#include <emmintrin.h>

namespace math {
    constexpr float pi{3.14159265358979323846f};
    constexpr float pi_2{pi * 2.0f};
    constexpr float pi_1divpi{1.0f / pi};
    constexpr float pi_1div2pi{1.0f / pi_2};
    constexpr float pi_div2{pi / 2.0f};
    constexpr float pi_div4{pi / 4.0f};
    constexpr float rad_pi{57.295779513082f};
    constexpr long double pi_rad{0.01745329251f};

    constexpr float deg_to_rad(const float val) {
        return val * (pi / 180.0f);
    }

    constexpr float rad_to_deg(const float val) {
        return val * (180.0f / pi);
    }

    template <typename type = float>
    type min(const type &a, const type &b) {
        static_assert(std::is_arithmetic<type>::value, "math::min only supports integral types.");
        return (type) _mm_cvtss_f32(_mm_min_ss(_mm_set_ss(float(a)), _mm_set_ss(float(b))));
    }

    template <typename type = float>
    type max(const type &a, const type &b) {
        static_assert(std::is_arithmetic<type>::value, "math::max only supports integral types.");

        return (type) _mm_cvtss_f32(_mm_max_ss(_mm_set_ss(float(a)), _mm_set_ss(float(b))));
    }

    template <typename type = float>
    void clamp(type &val, const type &min, const type &max) {
        static_assert(std::is_arithmetic<type>::value, "math::clamp only supports integral types.");
        val = static_cast<type>(
            _mm_cvtss_f32(_mm_min_ss(_mm_max_ss(_mm_set_ss(float(val)), _mm_set_ss(float(min))), _mm_set_ss(float(max)))));
    }

    template <typename type = float>
    type sqrt(const type &val) {
        static_assert(std::is_arithmetic<type>::value, "math::sqrt only supports integral types.");
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(float(val))));
    }

    bool clamp_angles(vector_t &angles);
    bool normalize_angles(vector_t &angles);
    float normalize_angle(float angle);
    float strafe_opti_normalize_angle(float angle, float max);
    float normalized_angle(float angle);
    float normalize_yaw(float angle);
    void angle_matrix(const vector_t &angles, matrix3x4_t &matrix);
    vector_t to_angles(const vector_t &vec);
    void sin_cos(float radian, float *sin, float *cos);
    void angle_to_vector(const vector_t &angles, vector_t &forward);
    vector_t angle_to_vector(const vector_t &angles);
    void matrix_position(const matrix3x4_t &matrix, vector_t &out);
    void angle_to_vectors(const vector_t &angles, vector_t *forward, vector_t *right, vector_t *up);
    vector_t vector_angles(const vector_t &start, const vector_t &end);
    vector_t vector_to_angles(const vector_t &start, const vector_t &end);
    vector_t vector_to_angles(const vector_t &v);
    vector_t vector_transform(const vector_t &in, const matrix3x4_t &matrix);
    float approach_angle(float to, float from, float speed);
    vector_t get_matrix_position(const matrix3x4_t &src);
    void set_matrix_position(vector_t pos, matrix3x4_t &matrix);
    vector_t dist_segment_to_segment(vector_t s1, vector_t s2, vector_t k1, vector_t k2);
    bool world_to_screen(const vector_t &world_pos, point_t &screen_pos);
    vector_t align_with_world(vector_t view);
    vector_t make_vector(const vector_t angles);
    void angle_vectors(const vector_t &angles, vector_t *forward, vector_t *right = nullptr, vector_t *up = nullptr);

    __forceinline vector_t calc_angle(const vector_t v1, const vector_t v2) {
        const vector_t delta = v1 - v2;

        if (delta.z == 0.0f && delta.length() == 0.0f) {
            return {};
        }

        if (delta.y == 0.0f && delta.x == 0.0f) {
            return {};
        }

        vector_t angles = {std::asin(delta.z / delta.length()) * rad_pi, std::atan(delta.y / delta.x) * rad_pi, 0.0f};

        if (delta.x >= 0.0f) {
            angles.y += 180.0f;
        }

        return angles;
    }

    __forceinline void vector_transform(const vector_t &in, const matrix3x4_t &matrix, vector_t &out) {
        out.x = in.dot(matrix.m_mat_val[0]) + matrix.m_mat_val[0][3];
        out.y = in.dot(matrix.m_mat_val[1]) + matrix.m_mat_val[1][3];
        out.z = in.dot(matrix.m_mat_val[2]) + matrix.m_mat_val[2][3];
    }

    __forceinline float get_fov(const vector_t &view_angs, const vector_t &aim_angles) {
        vector_t ang, aim;

        angle_to_vector(view_angs, aim);
        angle_to_vector(aim_angles, ang);

        return rad_to_deg(std::acos(aim.dot(ang) / aim.length_sqr()));
    }

    template <typename type = float>
    type ease_in(const type &t1, const type &t2, float progress, const float exp = 2.0f) {
        progress = std::pow(progress, exp);
        return t1 + (t2 - t1) * progress;
    }

    template <typename type = float>
    type ease_out(const type &t1, const type &t2, float progress) {
        progress = progress * (2.0f - progress);
        return t1 + (t2 - t1) * progress;
    }

    template <typename type = float>
    type ease_inout(const type &t1, const type &t2, float progress) {
        progress = progress < 0.5f ? 2.0f * progress * progress : -1.0f + (4.0f - 2.0f * progress) * progress;
        return t1 + (t2 - t1) * progress;
    }

    template <typename t>
    __forceinline t lerp(const t &t1, const t &t2, float progress) {
        return t1 + (t2 - t1) * progress;
    }

    template <typename T>
    constexpr T map_range(T value, T input_min, T input_max, T output_min, T output_max) {
        static_assert(std::is_arithmetic_v<T>, "type isn't arithmetic");

        const auto slope = (output_max - output_min) / (input_max - input_min);
        const auto output = output_min + slope * (value - input_min);

        return output;
    }
} // namespace math