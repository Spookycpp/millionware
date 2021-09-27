#pragma once

#include "color.h"
#include "input.h"
#include "key_values.h"
#include "macros.h"

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

class c_material_var {
public:
    DECLARE_VFUNC(4, set(float value), void(__thiscall *)(void *, float))(value);
    DECLARE_VFUNC(5, set(int value), void(__thiscall *)(void *, int))(value);
    DECLARE_VFUNC(6, set(const char *value), void(__thiscall *)(void *, const char *))(value);
    DECLARE_VFUNC(11, set(vector_t value), void(__thiscall *)(void *, float, float, float))(value.x, value.y, value.z);
    DECLARE_VFUNC(26, set_component(float value, int component), void(__thiscall *)(void *, float, int))(value, component);
};

class c_material {
public:
    DECLARE_VFUNC(0, get_name(), const char *(__thiscall *) (void *) )();
    DECLARE_VFUNC(1, get_group_name(), const char *(__thiscall *) (void *) )();
    DECLARE_VFUNC(11, find_var(const char *name, bool *found, bool complain = true),
                  c_material_var *(__thiscall *) (void *, const char *, bool *, bool) )
    (name, found, complain);
    DECLARE_VFUNC(12, increment_reference_count(), void(__thiscall *)(void *))();
    DECLARE_VFUNC(13, decrement_reference_count(), void(__thiscall *)(void *))();
    DECLARE_VFUNC(27, set_alpha(int alpha), void(__thiscall *)(void *, float))(static_cast<float>(alpha) / 255.f);
    DECLARE_VFUNC(28, set_color(float r, float g, float b), void(__thiscall *)(void *, float, float, float))(r, g, b);
    DECLARE_VFUNC(29, set_flag(int flag, bool state), void(__thiscall *)(void *, int, bool))(flag, state);
    DECLARE_VFUNC(34, set_shader(const char *name), void(__thiscall *)(void *, const char *))(name);
    DECLARE_VFUNC(37, refresh_material(), void(__thiscall *)(void *))();
    DECLARE_VFUNC(42, is_error_material(), bool(__thiscall *)(void *))();
    DECLARE_VFUNC(48, set_shader_and_parameters(c_key_values *key_value), void(__thiscall *)(void *, c_key_values *))(key_value);

    void set_color(const color_t &color) {
        set_color(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f);
    }
};