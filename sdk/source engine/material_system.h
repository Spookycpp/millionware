#pragma once

#include "macros.h"
#include "material.h"
#include "material_handle.h"

#define SV_CHECK(sv) (static_cast<decltype(sv.data())>(sv.empty() ? nullptr : sv.data()))

enum e_material_override_type {
    OVERRIDE_TYPE_NORMAL,
    OVERRIDE_TYPE_BUILD_SHADOWS,
    OVERRIDE_TYPE_DEPTH_WRITE,
    OVERRIDE_TYPE_WHATEVER,
};

class c_material_system {
public:
    DECLARE_VFUNC(83, create_material(const char *name, void *key_values), c_material *(__thiscall *) (void *, const char *, void *) )
    (name, key_values);
    DECLARE_VFUNC(84,
                  find_material(const char *name, const char *group_name = nullptr, bool complain = true,
                                const char *complain_prefix = nullptr),
                  c_material *(__thiscall *) (void *, const char *, const char *, bool, const char *) )
    (name, group_name, complain, complain_prefix);
    DECLARE_VFUNC(86, first_material(), uint16_t(__thiscall *)(void *))();
    DECLARE_VFUNC(87, next_material(uint16_t handle), uint16_t(__thiscall *)(void *, uint16_t))(handle);
    DECLARE_VFUNC(88, invalid_material(), uint16_t(__thiscall *)(void *))();
    DECLARE_VFUNC(89, get_material(uint16_t handle), c_material *(__thiscall *) (void *, uint16_t))(handle);
    DECLARE_VFUNC(90, get_material_count(), int(__thiscall *)(void *))();
};