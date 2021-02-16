#include <MinHook.h>
#include <stdio.h>
#include <Windows.h>

#include "../core/config.hpp"

#include "../utils/util.hpp"
#include "../utils/hash/hash.hpp"
#include "../utils/hook/hook.hpp"
#include "../utils/xorstr/xorstr.hpp"

#include "cheat.hpp"
#include "hooks.hpp"
#include "interfaces.hpp"
#include "patterns.hpp"

template <typename T>
inline uintptr_t get_vfunc_address(T* base_interface, size_t index) {
	return reinterpret_cast<uintptr_t**>(base_interface)[0][index];
}

void hooks::initialize() {
	// initialize stuff initialized by hooks that aren't ran immediately
	if (interfaces::engine_client->is_in_game())
		cheat::local_player = interfaces::entity_list->get_by_index(interfaces::engine_client->get_local_player())->as_player();

	// add hooks
	create_move = c_hook(get_vfunc_address(interfaces::client_mode, 24), reinterpret_cast<uintptr_t>(&create_move_hook));
	override_view = c_hook(get_vfunc_address(interfaces::client_mode, 18), reinterpret_cast<uintptr_t>(&override_view_hook));
	override_mouse_input = c_hook(get_vfunc_address(interfaces::client_mode, 23), reinterpret_cast<uintptr_t>(&override_mouse_input_hook));
	is_playing_demo = c_hook(get_vfunc_address(interfaces::engine_client, 82), reinterpret_cast<uintptr_t>(&is_playing_demo_hook));
	override_config = c_hook(get_vfunc_address(interfaces::material_system, 21), reinterpret_cast<uintptr_t>(&override_config_hook));
	level_init_post_entity = c_hook(get_vfunc_address(interfaces::client, 6), reinterpret_cast<uintptr_t>(&level_init_post_entity_hook));
	level_shutdown_pre_entity = c_hook(get_vfunc_address(interfaces::client, 7), reinterpret_cast<uintptr_t>(&level_shutdown_pre_entity_hook));
	frame_stage_notify = c_hook(get_vfunc_address(interfaces::client, 37), reinterpret_cast<uintptr_t>(&frame_stage_notify_hook));
	engine_paint = c_hook(get_vfunc_address(interfaces::engine_vgui, 14), reinterpret_cast<uintptr_t>(&engine_paint_hook));
	lock_cursor = c_hook(get_vfunc_address(interfaces::vgui_surface, 67), reinterpret_cast<uintptr_t>(&lock_cursor_hook));
	screen_size_changed = c_hook(get_vfunc_address(interfaces::vgui_surface, 116), reinterpret_cast<uintptr_t>(&screen_size_changed_hook));
	emit_sound = c_hook(get_vfunc_address(interfaces::engine_sound, 5), reinterpret_cast<uintptr_t>(&emit_sound_hook));
	get_screen_aspect_ratio = c_hook(get_vfunc_address(interfaces::engine_client, 101), reinterpret_cast<uintptr_t>(&get_screen_aspect_ratio_hook));
	draw_model_execute = c_hook(get_vfunc_address(interfaces::model_render, 21), reinterpret_cast<uintptr_t>(&draw_model_execute_hook));
	send_datagram = c_hook(patterns::send_datagram, 46), reinterpret_cast<uintptr_t>(&send_datagram_hook);
	is_connected = c_hook(get_vfunc_address(interfaces::engine_client, 27), reinterpret_cast<uintptr_t>(&is_connected_hook));
	draw_print_text = c_hook(get_vfunc_address(interfaces::vgui_surface, 28), reinterpret_cast<uintptr_t>(&draw_print_text_hook));

	// enable hooks
	create_move.enable();
	override_view.enable();
	override_mouse_input.enable();
	is_playing_demo.enable();
	override_config.enable();
	level_init_post_entity.enable();
	level_shutdown_pre_entity.enable();
	frame_stage_notify.enable();
	engine_paint.enable();
	lock_cursor.enable();
	screen_size_changed.enable();
	emit_sound.enable();
	get_screen_aspect_ratio.enable();
	draw_model_execute.enable();
	send_datagram.enable();
	is_connected.enable();
	draw_print_text.enable();
}

void hooks::shutdown() {
	// disable hooks
	create_move.disable();
	override_view.disable();
	override_mouse_input.disable();
	is_playing_demo.disable();
	override_config.disable();
	level_init_post_entity.disable();
	level_shutdown_pre_entity.disable();
	frame_stage_notify.disable();
	engine_paint.disable();
	lock_cursor.disable();
	screen_size_changed.disable();
	emit_sound.disable();
	get_screen_aspect_ratio.disable();
	draw_model_execute.disable();
	send_datagram.disable();
	is_connected.disable();
	draw_print_text.disable();

	// revert what hooks might've possibly messed up
	interfaces::input_system->enable_input(true);
	interfaces::convar_system->find(STR_ENC("r_aspectratio"))->set_value(0.0f);
	interfaces::convar_system->find(STR_ENC("@panorama_disable_blur"))->set_value(false);
	interfaces::convar_system->find(STR_ENC("mat_postprocess_enable"))->set_value(true);
	
	config::uninitialize();
}