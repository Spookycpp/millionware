#pragma once

struct color_t
{
	int r, g, b, a;

	constexpr color_t(int r, int g, int b) :r(r), g(g), b(b), a(255)
	{
	}

	constexpr color_t(int r, int g, int b, int a) : r(r), g(g), b(b), a(a)
	{
	}

	constexpr color_t adjust_alpha(int alpha) const
	{
		return { r, g, b, alpha };
	}
};
