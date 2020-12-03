#pragma once

#include "macros.hpp"
#include "material.hpp"

class c_model_render {
public:
  VIRTUAL_METHOD(void, force_material_override, 1, (material_t* material, int override_type = OVERRIDE_TYPE_NORMAL, int material_index = -1), material, override_type, material_index);
  VIRTUAL_METHOD(bool, is_forced_material_override, 2, ());
};
