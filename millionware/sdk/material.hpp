#pragma once

#include "color.hpp"
#include "macros.hpp"
#include "vector.hpp"

enum e_material_flag {
	MATERIAL_FLAG_DEBUG = 1 << 0,
	MATERIAL_FLAG_NO_DEBUG_OVERRIDE = 1 << 1,
	MATERIAL_FLAG_NO_DRAW = 1 << 2,
	MATERIAL_FLAG_USE_IN_FILLRATE_MODE = 1 << 3,
	MATERIAL_FLAG_VERTEX_COLOR = 1 << 4,
	MATERIAL_FLAG_VERTEX_ALPHA = 1 << 5,
	MATERIAL_FLAG_SELFILLUM = 1 << 6,
	MATERIAL_FLAG_ADDITIVE = 1 << 7,
	MATERIAL_FLAG_ALPHA_TEST = 1 << 8,
	MATERIAL_FLAG_MULTI_PASS = 1 << 9,
	MATERIAL_FLAG_Z_NEARER = 1 << 10,
	MATERIAL_FLAG_MODEL = 1 << 11,
	MATERIAL_FLAG_FLAT = 1 << 12,
	MATERIAL_FLAG_NO_CULL = 1 << 13,
	MATERIAL_FLAG_NO_FOG = 1 << 14,
	MATERIAL_FLAG_IGNORE_Z = 1 << 15,
	MATERIAL_FLAG_DECAL = 1 << 16,
	MATERIAL_FLAG_ENVMAP_SPHERE = 1 << 17,
	MATERIAL_FLAG_NO_ALPHA_MOD = 1 << 18,
	MATERIAL_FLAG_ENVMAP_CAMERA_SPACE = 1 << 19,
	MATERIAL_FLAG_BASE_ALPHA_ENVMAP_MASK = 1 << 20,
	MATERIAL_FLAG_TRANSLUCENT = 1 << 21,
	MATERIAL_FLAG_NORMAL_MAP_ALPHA_ENVMAP_MASK = 1 << 22,
	MATERIAL_FLAG_NEEDS_SOFTWARE_SKINNING = 1 << 23,
	MATERIAL_FLAG_OPAQUE_TEXTURE = 1 << 24,
	MATERIAL_FLAG_ENVMAP_MODE = 1 << 25,
	MATERIAL_FLAG_SUPPRESS_DECALS = 1 << 26,
	MATERIAL_FLAG_HALF_LAMBERT = 1 << 27,
	MATERIAL_FLAG_WIREFRAME = 1 << 28,
	MATERIAL_FLAG_ALLOW_ALPHA_TO_COVERAGE = 1 << 29,
	MATERIAL_FLAG_IGNORE_ALPHA_MODULATION = 1 << 30,
};

struct material_var_t {
	VIRTUAL_METHOD(void, set, 4, (float value), value);
	VIRTUAL_METHOD(void, set, 5, (int value), value);
	VIRTUAL_METHOD(void, set, 6, (std::string_view value), value.data());
	VIRTUAL_METHOD(void, set, 11, (vector3_t value), value.x, value.y, value.z);
	VIRTUAL_METHOD(void, set_component, 26, (float value, int component), value, component);
};

struct material_t {
	VIRTUAL_METHOD(const char*, get_name, 0, ());
	VIRTUAL_METHOD(const char*, get_group_name, 1, ());
	VIRTUAL_METHOD(material_var_t*, find_var, 11, (std::string_view name, bool& found, bool complain = true), name.data(), &found, complain); // 11
	VIRTUAL_METHOD(void, increment_reference_count, 12, ());
	VIRTUAL_METHOD(void, decrement_reference_count, 13, ());
	VIRTUAL_METHOD(void, set_alpha, 27, (float alpha));
	VIRTUAL_METHOD(void, set_color, 28, (float r, float g, float b), r, g, b);
	VIRTUAL_METHOD(void, set_color, 28, (color_t color), static_cast<float>(color.r / 255.0f), static_cast<float>(color.g / 255.0f), static_cast<float>(color.b / 255.0f));
	VIRTUAL_METHOD(void, set_flag, 29, (int flag, bool state), flag, state);
	VIRTUAL_METHOD(void, refresh, 37, ());
};
