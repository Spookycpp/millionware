#pragma once

#include "macros.h"
#include "material.h"
#include "material_system.h"

class c_model_render {
public:
    DECLARE_VFUNC(1, force_material_override(c_material *material, int override_type = OVERRIDE_TYPE_NORMAL, int material_index = -1),
                  void(__thiscall *)(void *, c_material *, int, int))
    (material, override_type, material_index);
    DECLARE_VFUNC(2, is_forced_material_override(), bool(__thiscall *)(void *))();
};