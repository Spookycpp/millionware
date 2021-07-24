#pragma once

#include "../lua_internal.hpp"


struct color {
    int r, g, b, a;

    constexpr color() : r(255), g(255), b(255), a(255) {}

    constexpr color(int scalar) : r(scalar), g(scalar), b(scalar), a(255) {}

    constexpr color(int scalar, int a) : r(scalar), g(scalar), b(scalar), a(a) {}

    constexpr color(int r, int g, int b) : r(r), g(g), b(b), a(255) {}

    constexpr color(int r, int g, int b, int a) : r(r), g(g), b(b), a(a) {}

    constexpr color adjust_alpha(int alpha) const {
        return color(r, g, b, alpha);
    }

    constexpr float luminance() const {
        return (0.299f * r + 0.587f * g + 0.114f * b) / 255.0f;
    }

    static color hsb(const float hue, const float saturation, const float brightness) {
        float h = hue == 1.0f ? 0 : hue * 6.0f;
        float f = h - static_cast<int>(h);
        float p = brightness * (1.0f - saturation);
        float q = brightness * (1.0f - saturation * f);
        float t = brightness * (1.0f - (saturation * (1.0f - f)));

        if (h < 1) {
            return color(static_cast<unsigned char>(brightness * 255), static_cast<unsigned char>(t * 255), static_cast<unsigned char>(p * 255));
        }
        if (h < 2) {
            return color(static_cast<unsigned char>(q * 255), static_cast<unsigned char>(brightness * 255), static_cast<unsigned char>(p * 255));
        }
        if (h < 3) {
            return color(static_cast<unsigned char>(p * 255), static_cast<unsigned char>(brightness * 255), static_cast<unsigned char>(t * 255));
        }
        if (h < 4) {
            return color(static_cast<unsigned char>(p * 255), static_cast<unsigned char>(q * 255), static_cast<unsigned char>(brightness * 255));
        }
        if (h < 5) {
            return color(static_cast<unsigned char>(t * 255), static_cast<unsigned char>(p * 255), static_cast<unsigned char>(brightness * 255));
        }
        return color(static_cast<unsigned char>(brightness * 255), static_cast<unsigned char>(p * 255), static_cast<unsigned char>(q * 255));
    }
};

inline void lua_color(lua_State* l) {
    luabridge::getGlobalNamespace(l)
    .beginClass<color>(xs("color"))
        .addConstructor<void(*)(const int&, const int&, const int&, const int&)>()
        .addData(xs("r"), &color::r)
        .addData(xs("g"), &color::g)
        .addData(xs("b"), &color::b)
        .addData(xs("a"), &color::a)
        .addStaticFunction("hsb", &color::hsb)
	.endClass();
}