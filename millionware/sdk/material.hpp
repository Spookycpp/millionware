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

struct material_video_mode {
	int m_Width;
	int m_Height;
	int m_Format;
	int m_RefreshRate;
};

struct c_material_system_config {
	material_video_mode video_mode;
	float               monitor_gamma;
	float               gamma_tv_range_min;
	float               gamma_tv_range_max;
	float               gamma_tv_exponent;
	bool                gamma_tv_enabled;
	bool                triple_buffered;
	int                 aa_samples;
	int                 force_anisotropic_level;
	int                 skip_mip_levels;
	int                 dx_support_level;
	int                 flags;
	bool                edit_mode;
	char                proxies_test_mode;
	bool                compressed_textures;
	bool                filter_lightmaps;
	bool                filter_textures;
	bool                reverse_depth;
	bool                buffer_primitives;
	bool                draw_flat;
	bool                measure_fill_rate;
	bool                visualize_fill_rate;
	bool                no_transparency;
	bool                software_lighting;
	bool                allow_cheats;
	char                show_mip_levels;
	bool                show_low_res_image;
	bool                show_normal_map;
	bool                mip_map_textures;
	char                full_bright;
	bool                fast_no_bump;
	bool                suppress_rendering;
	bool                draw_gray;
	bool                show_specular;
	bool                show_diffuse;
	int                 windowed_size_limit_width;
	int                 windowed_size_limit_height;
	int                 aa_quality;
	bool                shadow_depth_texture;
	bool                motion_blur;
	bool                support_flash_light;
	bool                paint_enabled;
	char padding_[0xC];
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
	VIRTUAL_METHOD(material_var_t*, find_var, 11, (std::string_view name, bool& found, bool complain = true), name.data(), found, complain); // 11
	VIRTUAL_METHOD(void, increment_reference_count, 12, ());
	VIRTUAL_METHOD(void, decrement_reference_count, 13, ());
	VIRTUAL_METHOD(void, set_alpha, 27, (float alpha));
	VIRTUAL_METHOD(void, set_color, 28, (float r, float g, float b), r, g, b);
	VIRTUAL_METHOD(void, set_color, 28, (color_t color), static_cast<float>(color.r * 255.0f), static_cast<float>(color.g * 255.0f), static_cast<float>(color.b * 255.0f));
	VIRTUAL_METHOD(void, set_flag, 29, (int flag, bool state), flag, state);
	VIRTUAL_METHOD(void, refresh, 37, ());
};
