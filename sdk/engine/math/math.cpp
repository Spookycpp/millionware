#include "math.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/patterns/patterns.h"
#include "../../source engine/matrix.h"
#include "../render/render.h"

#include <algorithm>

namespace math {

    bool clamp_angles(vector_t &angles) {
        if (std::isfinite(angles.x) && std::isfinite(angles.y) && std::isfinite(angles.z)) {
            angles.x = std::clamp(angles.x, -89.0f, 89.0f);
            angles.y = std::clamp(angles.y, -180.0f, 180.0f);
            angles.z = 0.0f;

            return true;
        }

        return false;
    }

    bool normalize_angles(vector_t &angles) {
        if (std::isfinite(angles.x) && std::isfinite(angles.y) && std::isfinite(angles.z)) {
            angles.x = std::remainder(angles.x, 360.0f);
            angles.y = std::remainder(angles.y, 360.0f);

            return true;
        }

        return false;
    }

    float normalize_angle(const float angle) {
        if (!std::isfinite(angle)) {
            return 0.0f;
        }

        return std::remainder(angle, 360.0f);
    }

    float strafe_opti_normalize_angle(float angle, float max) {
        while (angle > max)
            angle -= max * 2;
        while (angle < -max)
            angle += max * 2;
        return angle;
    }

    float normalized_angle(float angle) {
        normalize_angle(angle);
        return angle;
    }

    float normalize_yaw(float angle) {
        if (!std::isfinite(angle))
            angle = 0.f;

        return std::remainderf(angle, 360.0f);
    }

    vector_t to_angles(const vector_t &vec) {
        vector_t angs;

        if (vec.x == 0.0f && vec.y == 0.0f) {
            angs.x = (vec.z > 0.0f) ? 270.0f : 90.0f;
            angs.y = 0.0f;
        } else {
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

    void sin_cos(const float radian, float *sin, float *cos) {
        *sin = std::sin(radian);
        *cos = std::cos(radian);
    }

    void angle_to_vector(const vector_t &angles, vector_t &forward) {
        float sp, sy, cp, cy;

        sin_cos(deg_to_rad(angles.y), &sy, &cy);
        sin_cos(deg_to_rad(angles.x), &sp, &cp);

        forward.x = cp * cy;
        forward.y = cp * sy;
        forward.z = -sp;
    }

    vector_t angle_to_vector(const vector_t &angles) {
        float sp, sy, cp, cy;

        sin_cos(deg_to_rad(angles.y), &sy, &cy);
        sin_cos(deg_to_rad(angles.x), &sp, &cp);

        return {cp * cy, cp * sy, -sp};
    }

    void matrix_position(const matrix3x4_t &matrix, vector_t &out) {
        out.x = matrix.m_mat_val[0][3];
        out.y = matrix.m_mat_val[1][3];
        out.z = matrix.m_mat_val[2][3];
    }

    void angle_to_vectors(const vector_t &angles, vector_t *forward, vector_t *right, vector_t *up) {
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

    vector_t vector_angles(const vector_t &start, const vector_t &end) {
        const vector_t delta = end - start;

        const float magnitude = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        const float pitch = std::atan2(-delta.z, magnitude) * rad_pi;
        const float yaw = std::atan2(delta.y, delta.x) * rad_pi;

        return vector_t(pitch, yaw, 0.0f);
    }

    vector_t vector_to_angles(const vector_t &start, const vector_t &end) {
        const vector_t delta = end - start;

        const float magnitude = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        const float pitch = std::atan2(-delta.z, magnitude) * rad_pi;
        const float yaw = std::atan2(delta.y, delta.x) * rad_pi;

        return vector_t(pitch, yaw, 0.0f);
    }

    vector_t vector_to_angles(const vector_t &v) {
        const float magnitude = std::sqrt(v.x * v.x + v.y * v.y);
        const float pitch = std::atan2(-v.z, magnitude) * rad_pi;
        const float yaw = std::atan2(v.y, v.x) * rad_pi;

        return vector_t(pitch, yaw, 0.0f);
    }

    void angle_matrix(const vector_t &angles, matrix3x4_t &matrix) {
        float sr, sp, sy, cr, cp, cy;

        sin_cos(deg_to_rad(angles.x), &sp, &cp);
        sin_cos(deg_to_rad(angles.y), &sy, &cy);
        sin_cos(deg_to_rad(angles.z), &sr, &cr);

        matrix[0][0] = cp * cy;
        matrix[1][0] = cp * sy;
        matrix[2][0] = -sp;

        const float crcy = cr * cy, crsy = cr * sy, srcy = sr * cy, srsy = sr * sy;

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

    vector_t vector_transform(const vector_t &in, const matrix3x4_t &matrix) {
        return vector_t(in.dot(matrix[0]) + matrix[0][3], in.dot(matrix[1]) + matrix[1][3], in.dot(matrix[2]) + matrix[2][3]);
    }

    float approach_angle(const float to, float from, const float speed) {
        const float delta = std::remainder(to - from, 360.0f);

        if (delta > speed) {
            from += speed;
        } else if (delta < -speed) {
            from -= speed;
        } else {
            from = to;
        }

        return std::clamp(from, -180.0f, 180.0f);
    }

    vector_t get_matrix_position(const matrix3x4_t &src) {
        return vector_t(src[0][3], src[1][3], src[2][3]);
    }

    void set_matrix_position(vector_t pos, matrix3x4_t &matrix) {
        for (size_t i = 0; i < 3; ++i) {
            matrix[i][3] = ((float *) &pos)[i];
        }
    }

    vector_t dist_segment_to_segment(const vector_t s1, const vector_t s2, const vector_t k1, const vector_t k2) {
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
        } else {
            sN = (b * e - c * d);
            tN = (a * e - b * d);
            if (sN < 0.0f) {
                sN = 0.0f;
                tN = e;
                tD = c;
            } else if (sN > sD) {
                sN = sD;
                tN = e + b;
                tD = c;
            }
        }

        if (tN < 0.0f) {
            tN = 0.0;

            if (-d < 0.0f) {
                sN = 0.0;
            } else if (-d > a) {
                sN = sD;
            } else {
                sN = -d;
                sD = a;
            }
        } else if (tN > tD) {
            tN = tD;

            if (-d + b < 0.0f) {
                sN = 0;
            } else if (-d + b > a) {
                sN = sD;
            } else {
                sN = -d + b;
                sD = a;
            }
        }

        const float sc = std::abs(sN) < SMALL_NUM ? 0.0f : sN / sD;
        const float tc = std::abs(tN) < SMALL_NUM ? 0.0f : tN / tD;

        return w + u * sc - v * tc;
    }

    bool world_to_screen(const vector_t &world_pos, point_t &screen_pos) {
        /*
        static matrix4x4_t *view = nullptr;

        if (!view) {
            auto address = patterns::get_view_matrix();

            address = (*(uint32_t *) (address + 3)) + 176;
            view = (matrix4x4_t *) address;
        }*/

        matrix4x4_t &view_matrix = cheat::view_matrix;

        const auto x =
            view_matrix[0][0] * world_pos.x + view_matrix[0][1] * world_pos.y + view_matrix[0][2] * world_pos.z + view_matrix[0][3];
        const auto y =
            view_matrix[1][0] * world_pos.x + view_matrix[1][1] * world_pos.y + view_matrix[1][2] * world_pos.z + view_matrix[1][3];
        const auto w =
            view_matrix[3][0] * world_pos.x + view_matrix[3][1] * world_pos.y + view_matrix[3][2] * world_pos.z + view_matrix[3][3];

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

    vector_t align_with_world(vector_t view) {
        vector_t aligned_view = {0.0f, 0.0f, 0.0f};

        float yaw = view.y;

        if (yaw >= 45.0f && yaw <= 135.0f) {
            aligned_view.y = 90.0f;
        } else if (yaw <= 45.0f && yaw >= -45.0f) {
            aligned_view.y = 0.0f;
        } else if (yaw <= -45.0f && yaw <= -135.0f) {
            aligned_view.y = -90.0f;
        } else {
            aligned_view.y = 180.0f;
        }

        return aligned_view;
    }

    vector_t make_vector(const vector_t angles) {
        float cp = cos(deg_to_rad(angles.y));
        float sy = sin(deg_to_rad(angles.y));

        float sp = sin(deg_to_rad(angles.x));
        float cy = cos(deg_to_rad(angles.x));

        return {(cp * cy), (cp * sy), (-sp)};
    }

    void angle_vectors(const vector_t &angles, vector_t *forward, vector_t *right, vector_t *up) {
        float cp = cos(deg_to_rad(angles.x)), sp = sin(deg_to_rad(angles.x));
        float cy = cos(deg_to_rad(angles.y)), sy = sin(deg_to_rad(angles.y));
        float cr = cos(deg_to_rad(angles.z)), sr = sin(deg_to_rad(angles.z));

        if (forward) {
            forward->x = cp * cy;
            forward->y = cp * sy;
            forward->z = -sp;
        }

        if (right) {
            right->x = -1.f * sr * sp * cy + -1.f * cr * -sy;
            right->y = -1.f * sr * sp * sy + -1.f * cr * cy;
            right->z = -1.f * sr * cp;
        }

        if (up) {
            up->x = cr * sp * cy + -sr * -sy;
            up->y = cr * sp * sy + -sr * cy;
            up->z = cr * cp;
        }
    }

} // namespace math