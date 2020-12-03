#pragma once

#include "macros.hpp"
#include "material.hpp"
#include "material_handle.hpp"
#include "typedefs.hpp"

#define SV_CHECK(sv) (static_cast<decltype(sv.data())>(sv.empty() ? nullptr : sv.data()))

enum e_material_override_type {
	OVERRIDE_TYPE_NORMAL,
	OVERRIDE_TYPE_BUILD_SHADOWS,
	OVERRIDE_TYPE_DEPTH_WRITE,
	OVERRIDE_TYPE_WHATEVER,
};

class c_material_system {
public:
	VIRTUAL_METHOD(material_t*, create_material, 83, (std::string_view name, uintptr_t key_values), name.data(), key_values);
	VIRTUAL_METHOD(material_t*, get_material, 84, (std::string_view material_name, std::string_view group_name = "", bool complain = true, std::string_view complain_prefix = ""), SV_CHECK(material_name), SV_CHECK(group_name), complain, SV_CHECK(complain_prefix));
	VIRTUAL_METHOD(material_handle_t, first_material, 86, ());
	VIRTUAL_METHOD(material_handle_t, next_material, 87, (material_handle_t handle), handle.handle);
	VIRTUAL_METHOD(material_handle_t, invalid_material, 88, ());
	VIRTUAL_METHOD(material_t*, get_material, 89, (material_handle_t handle), handle.handle);
	VIRTUAL_METHOD(int, get_material_count, 90, ());
};
