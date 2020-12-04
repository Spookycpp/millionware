#include <MinHook.h>
#include <stdio.h>
#include <Windows.h>

#include "../utils/error.hpp"
#include "../utils/hash.hpp"
#include "../utils/xorstr.hpp"

#include "cheat.hpp"
#include "hooks.hpp"
#include "interfaces.hpp"
#include "patterns.hpp"

#define ADD_HOOK(storage)                                                                                 \
  if (MH_CreateHook(reinterpret_cast<LPVOID>(storage.function), reinterpret_cast<LPVOID>(storage.detour), \
                    reinterpret_cast<LPVOID*>(&storage.original)) != MH_OK)                               \
  {                                                                                                       \
    utils::error_and_exit(e_error_code::HOOKS, FNV_CT("set up " #storage));                               \
  }

#define REMOVE_HOOK(storage)                                                      \
  if (MH_RemoveHook(reinterpret_cast<LPVOID>(storage.function)) != MH_OK)         \
    utils::error_and_exit(e_error_code::HOOKS, FNV_CT("remove " #storage));       \
  const auto _##storage##_lock = std::lock_guard(storage.call_mutex);

template <typename T>
inline uintptr_t get_vfunc_address(T* base_interface, size_t index) {
	return reinterpret_cast<uintptr_t**>(base_interface)[0][index];
}

void hooks::initialize() {
	// initialize stuff initialized by hooks that aren't ran immediately
	if (interfaces::engine_client->is_in_game())
		cheat::local_player = interfaces::entity_list->get_by_index(interfaces::engine_client->get_local_player())->as_player();

	// initialize hooking engine
	if (MH_Initialize() != MH_OK)
		utils::error_and_exit(e_error_code::HOOKS, FNV_CT("minhook initialize"));

	// add hooks
	create_move.function = get_vfunc_address(interfaces::client_mode, 24);
	create_move.detour = reinterpret_cast<uintptr_t>(&create_move_hook);

	hk_is_playing_demo.function = get_vfunc_address(interfaces::engine_client, 82);
	hk_is_playing_demo.detour = reinterpret_cast<uintptr_t>(&playing_demo_hook);

	override_config.function = get_vfunc_address(interfaces::material_system, 21);
	override_config.detour = reinterpret_cast<uintptr_t>(&override_config_hook);

	level_init_post_entity.function = get_vfunc_address(interfaces::client, 6);
	level_init_post_entity.detour = reinterpret_cast<uintptr_t>(&level_init_post_entity_hook);

	level_shutdown_pre_entity.function = get_vfunc_address(interfaces::client, 7);
	level_shutdown_pre_entity.detour = reinterpret_cast<uintptr_t>(&level_shutdown_pre_entity_hook);

	frame_stage_notify.function = get_vfunc_address(interfaces::client, 37);
	frame_stage_notify.detour = reinterpret_cast<uintptr_t>(&frame_stage_notify_hook);

	engine_paint.function = get_vfunc_address(interfaces::engine_vgui, 14);
	engine_paint.detour = reinterpret_cast<uintptr_t>(&engine_paint_hook);

	lock_cursor.function = get_vfunc_address(interfaces::vgui_surface, 67);
	lock_cursor.detour = reinterpret_cast<uintptr_t>(&lock_cursor_hook);

	screen_size_changed.function = get_vfunc_address(interfaces::vgui_surface, 116);
	screen_size_changed.detour = reinterpret_cast<uintptr_t>(&screen_size_changed_hook);

	emit_sound.function = get_vfunc_address(interfaces::engine_sound, 5);
	emit_sound.detour = reinterpret_cast<uintptr_t>(&emit_sound_hook);

	get_screen_aspect_ratio.function = get_vfunc_address(interfaces::engine_client, 101);
	get_screen_aspect_ratio.detour = reinterpret_cast<uintptr_t>(&get_screen_aspect_ratio_hook);

	ADD_HOOK(create_move);
	ADD_HOOK(hk_is_playing_demo);
	ADD_HOOK(override_config);
	ADD_HOOK(level_init_post_entity);
	ADD_HOOK(level_shutdown_pre_entity);
	ADD_HOOK(frame_stage_notify);
	ADD_HOOK(engine_paint);
	ADD_HOOK(lock_cursor);
	ADD_HOOK(screen_size_changed);
	ADD_HOOK(emit_sound);
	ADD_HOOK(get_screen_aspect_ratio);

	if (MH_EnableHook(nullptr) != MH_OK)
		utils::error_and_exit(e_error_code::HOOKS, FNV_CT("enable all hooks"));
}

void hooks::shutdown() {
	if (MH_DisableHook(nullptr) != MH_OK)
		utils::error_and_exit(e_error_code::HOOKS, FNV_CT("disable all hooks"));

	REMOVE_HOOK(create_move);
	REMOVE_HOOK(hk_is_playing_demo);
	REMOVE_HOOK(override_config);
	REMOVE_HOOK(level_init_post_entity);
	REMOVE_HOOK(level_shutdown_pre_entity);
	REMOVE_HOOK(frame_stage_notify);
	REMOVE_HOOK(engine_paint);
	REMOVE_HOOK(lock_cursor);
	REMOVE_HOOK(screen_size_changed);
	REMOVE_HOOK(emit_sound);
	REMOVE_HOOK(get_screen_aspect_ratio);

	// lets hope no hooks are running :D
	if (MH_Uninitialize() != MH_OK)
		utils::error_and_exit(e_error_code::HOOKS, FNV_CT("minhook uninitialize"));

	// revert what hooks might've possibly messed up
	interfaces::input_system->enable_input(true);
	interfaces::convar_system->find(XOR("r_aspectratio"))->set_value(0.f);
	interfaces::convar_system->find(XOR("@panorama_disable_blur"))->set_value(false);
	interfaces::convar_system->find(XOR("mat_postprocess_enable"))->set_value(true);
}