#pragma once

#include <d3d9.h>

struct color_t {
	int r, g, b, a;

	constexpr color_t()
		: r(0), g(0), b(0), a(255) {}

	constexpr color_t(int scalar, int a = 255)
		: r(scalar), g(scalar), b(scalar), a(a) {}

	constexpr color_t(int r, int g, int b, int a = 255)
		: r(r), g(g), b(b), a(a) {}

	constexpr color_t adjust_alpha(int new_alpha) const {
		return color_t(r, g, b, new_alpha);
	}
};
