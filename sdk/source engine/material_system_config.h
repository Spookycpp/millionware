#pragma once

struct material_system_config_t {
	int width;
	int height;
	int format;
	int refresh_rate;
	float monitor_gamma;
	float gamma_tv_range_min;
	float gamma_tv_range_max;
	float gamma_tv_exponent;
	bool gamma_tv_enabled;
	bool triple_buffered;
	int aa_samples;
	int force_anisotropic_level;
	int skip_mip_levels;
	int dx_support_level;
	int flags;
	bool edit_mode;
	char proxies_test_mode;
	bool compressed_textures;
	bool filter_lightmaps;
	bool filter_textures;
	bool reverse_depth;
	bool buffer_primitives;
	bool draw_flat;
	bool measure_fill_rate;
	bool visualize_fill_rate;
	bool no_transparency;
	bool software_lighting;
	bool allow_cheats;
	char show_mip_levels;
	bool show_low_res_image;
	bool show_normal_map;
	bool mip_map_textures;
	char full_bright;
	bool fast_no_bump;
	bool suppress_rendering;
	bool draw_gray;
	bool show_specular;
	bool show_diffuse;
	int windowed_size_limit_width;
	int windowed_size_limit_height;
	int aa_quality;
	bool shadow_depth_texture;
	bool motion_blur;
	bool support_flash_light;
	bool paint_enabled;
};
