#pragma once

#include <mutex>

#include "../sdk/user_cmd.hpp"
#include "../sdk/material_system_config.hpp"
#include "../sdk/client_dll.hpp"
#include "../sdk/model_info.hpp"
#include "../utils/hook/hook.hpp"

namespace hooks {

	inline c_hook create_move;
	inline c_hook override_view;
	inline c_hook override_mouse_input;
	inline c_hook is_playing_demo;
	inline c_hook override_config;
	inline c_hook level_init_post_entity;
	inline c_hook level_shutdown_pre_entity;
	inline c_hook frame_stage_notify;
	inline c_hook engine_paint;
	inline c_hook lock_cursor;
	inline c_hook screen_size_changed;
	inline c_hook emit_sound;
	inline c_hook get_screen_aspect_ratio;
	inline c_hook draw_model_execute;

	extern bool __fastcall create_move_hook(uintptr_t, uintptr_t, float, user_cmd_t*);
	extern void __fastcall override_view_hook(uintptr_t, uintptr_t, view_setup_t*);
	extern void __fastcall override_mouse_input_hook(uintptr_t, uintptr_t, float* x, float* y);
	extern bool __fastcall is_playing_demo_hook(uintptr_t, uintptr_t);
	extern bool __fastcall override_config_hook(uintptr_t, uintptr_t, material_system_config_t*, bool);
	extern void __fastcall level_init_post_entity_hook(uintptr_t, uintptr_t);
	extern void __fastcall level_shutdown_pre_entity_hook(uintptr_t, uintptr_t);
	extern void __fastcall frame_stage_notify_hook(uintptr_t, uintptr_t, int);
	extern void __fastcall engine_paint_hook(uintptr_t, uintptr_t, int);
	extern void __fastcall lock_cursor_hook(uintptr_t, uintptr_t);
	extern void __fastcall screen_size_changed_hook(uintptr_t, uintptr_t, int, int);
	extern void __fastcall emit_sound_hook(uintptr_t, uintptr_t, uintptr_t, int, int, const char*, int, const char*, float, float, int, int, int, const vector3_t*, const vector3_t*, vector3_t*, bool, float, int, uintptr_t);
	extern float __fastcall get_screen_aspect_ratio_hook(uintptr_t, uintptr_t, int, int);
	extern void __fastcall draw_model_execute_hook(uintptr_t, uintptr_t, void*, void*, c_model_render_info*, matrix3x4_t*);

	void initialize();
	void shutdown();

}