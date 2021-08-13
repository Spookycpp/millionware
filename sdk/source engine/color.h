#pragma once

struct color_t {
    int r, g, b, a;

    constexpr color_t() : r(255), g(255), b(255), a(255) {}

    constexpr color_t(const int scalar) : r(scalar), g(scalar), b(scalar), a(255) {}

    constexpr color_t(const int scalar, int const a) : r(scalar), g(scalar), b(scalar), a(a) {}

    constexpr color_t(const int r, const int g, const int b) : r(r), g(g), b(b), a(255) {}

    constexpr color_t(const int r, const int g, const int b, const int a) : r(r), g(g), b(b), a(a) {}

    constexpr color_t adjust_alpha(const int alpha) const {
        return color_t(r, g, b, alpha);
    }

    constexpr float luminance() const {
        return (0.299f * r + 0.587f * g + 0.114f * b) / 255.0f;
    }

    bool operator!=(const color_t &src) const {
        return (src.r != this->r) || (src.g != this->g) || (src.b != this->b) || (src.a != this->a);
    }
};
