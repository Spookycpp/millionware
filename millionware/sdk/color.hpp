#pragma once

#include <d3d9.h>

struct color_t
{
  int r, g, b, a;

  constexpr color_t()
    : r(0), g(0), b(0), a(255) {
  }

  constexpr color_t(const int scalar, const int a = 255)
    : r(scalar), g(scalar), b(scalar), a(a) {
  }

  constexpr color_t(const int r, const int g, const int b, const int a = 255)
    : r(r), g(g), b(b), a(a) {
  }

  constexpr uint32_t to_d3d_color() const {
    return D3DCOLOR_RGBA(r, g, b, a);
  }

  constexpr color_t adjust_alpha(const int new_alpha) const {
    return color_t(r, g, b, new_alpha);
  }
};
