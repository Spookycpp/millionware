#include <cmath>

#include "easing.h"

constexpr static auto M_PI = 3.14159265358979323846f;

namespace easing {

    float in_sine(float t) {
        return sinf(1.5707963f * t);
    }

    float out_sine(float t) {
        return 1 + sinf(1.5707963f * (--t));
    }

    float in_out_sine(float t) {
        return 0.5f * (1.0f + sinf(3.1415926f * (t - 0.5f)));
    }

    float in_quad(float t) {
        return t * t;
    }

    float out_quad(float t) {
        return t * (2.0f - t);
    }

    float in_out_quad(float t) {
        return t < 0.5f ? 2.0f * t * t : t * (4.0f - 2.0f * t) - 1.0f;
    }

    float in_cubic(float t) {
        return t * t * t;
    }

    float out_cubic(float t) {
        return 1.0f + (--t) * t * t;
    }

    float in_out_cubic(float t) {
        return t < 0.5f ? 4.0f * t * t * t : 1.0f + (--t) * (2.0f * (--t)) * (2.0f * t);
    }

    float in_quart(float t) {
        return t * t * t * t;
    }

    float out_quart(float t) {
        t = (--t) * t;

        return 1.0f - t * t;
    }

    float in_out_quart(float t) {
        return t < 0.5f ? 8.0f * t * t : 1.0f - 8.0f * t * t;
    }

    float in_quint(float t) {
        const auto t2 = t * t;

        return t * t2 * t2;
    }

    float out_quint(float t) {
        const auto t2 = (--t) * t;

        return 1.0f + t * t2 * t2;
    }

    float in_out_quint(float t) {
        const auto t2 = t < 0.5f ? t * t : (--t) * t;

        return t < 0.5f ? 16.0f * t * t2 * t2 : 1 + 16.0f * t * t2 * t2;
    }

    float in_expo(float t) {
        return (powf(2.0f, 8.0f * t) - 1.0f) / 255.0f;
    }

    float out_expo(float t) {
        return 1.0f - powf(2.0f, -8.0f * t);
    }

    float in_out_expo(float t) {
        return t < 0.5f ? (powf(2.0f, 16.0f * t) - 1.0f) / 510.0f : 1.0f - 0.5f * powf(2.0f, -16.0f * (t - 0.5f));
    }

    float in_circ(float t) {
        return 1.0f - sqrtf(1.0f - t);
    }

    float out_circ(float t) {
        return sqrtf(t);
    }

    float in_out_circ(float t) {
        return t < 0.5f ? (1.0f - sqrtf(1.0f - 2.0f * t)) * 0.5f : (1.0f + sqrtf(2.0f * t - 1.0f)) * 0.5f;
    }

    float in_back(float t) {
        return t * t * (2.70158f * t - 1.70158f);
    }

    float out_back(float t) {
        return 1.0f + (--t) * t * (2.70158f * t + 1.70158f);
    }

    float in_out_back(float t) {
        return t < 0.5f ? t * t * (7.0f * t - 2.5f) * 2.0f : 1.0f + (--t) * t * 2.0f * (7.0f * t + 2.5f);
    }

    float in_elastic(float t) {
        const auto t2 = t * t;

        return t2 * t2 * sinf(t * M_PI * 4.5f);
    }

    float out_elastic(float t) {
        const auto t2 = (t - 1.0f) * (t - 1.0f);

        return 1.0f - t2 * t2 * cosf(t * M_PI * 4.5f);
    }

    float in_out_elastic(float t) {
        const auto t2 = t < 0.45f ? t * t : (t - 1.0f) * (t - 1.0f);

        if (t < 0.45f) {
            return 8.0f * t2 * t2 * sinf(t * M_PI * 9.0f);
        } else if (t < 0.55f) {
            return 0.5f + 0.75f * sinf(t * M_PI * 4.0f);
        } else {
            return 1.0f - 8.0f * t2 * t2 * sinf(t * M_PI * 9.0f);
        }
    }

    float in_bounce(float t) {
        return powf(2.0f, 6.0f * (t - 1.0f)) * fabsf(sinf(t * M_PI * 3.5f));
    }

    float out_bounce(float t) {
        return 1.0f - powf(2.0f, -6.0f * t) * fabsf(cosf(t * M_PI * 3.5f));
    }

    float in_out_bounce(float t) {
        return t < 0.5f ? 8.0f * powf(2.0f, 8.0f * (t - 1.0f)) * fabsf(sinf(t * M_PI * 7.0f))
                        : 1.0f - 8.0f * powf(2.0f, -8.0f * t) * fabsf(sinf(t * M_PI * 7.0f));
    }
} // namespace easing