#pragma once

#include <mutex>

#include "../sdk/user_cmd.hpp"
#include "../sdk/material_system_config.hpp"

struct hook_storage_t {
	uintptr_t function = 0;
	uintptr_t detour = 0;
	uintptr_t original = 0;

	std::mutex call_mutex;
};

namespace hooks {

	inline hook_storage_t create_move;
	inline hook_storage_t hk_is_playing_demo;
	inline hook_storage_t override_config;
	inline hook_storage_t level_init_post_entity;
	inline hook_storage_t level_shutdown_pre_entity;
	inline hook_storage_t frame_stage_notify;
	inline hook_storage_t engine_paint;
	inline hook_storage_t lock_cursor;
	inline hook_storage_t screen_size_changed;
	inline hook_storage_t emit_sound;

	extern bool __fastcall create_move_hook(uintptr_t, uintptr_t, float, user_cmd_t*);
	extern bool __fastcall playing_demo_hook(uintptr_t* ecx, uintptr_t* edx);
	extern bool __fastcall override_config_hook(uintptr_t, uintptr_t, material_system_config_t* config, bool update);
	extern void __fastcall level_init_post_entity_hook(uintptr_t, uintptr_t);
	extern void __fastcall level_shutdown_pre_entity_hook(uintptr_t, uintptr_t);
	extern void __fastcall frame_stage_notify_hook(uintptr_t, uintptr_t, int);
	extern void __fastcall engine_paint_hook(uintptr_t, uintptr_t, int);
	extern void __fastcall lock_cursor_hook(uintptr_t, uintptr_t);
	extern void __fastcall screen_size_changed_hook(uintptr_t, uintptr_t, int, int);
	extern void __fastcall emit_sound_hook(uintptr_t, uintptr_t, uintptr_t, int, int, const char*, int, const char*, float, float, int, int, int, const vector3_t*, const vector3_t*, vector3_t*, bool, float, int, uintptr_t);

	void initialize();
	void shutdown();

}
