#pragma once

#include <cstdint>
#include <d3d9.h>
#include <memory>

#include "../../engine/hash/hash.h"
#include "../../source engine/client_dll.h"
#include "../../source engine/engine_client.h"
#include "../../source engine/input.h"
#include "../../source engine/vgui.h"
#include "../../source engine/view_setup.h"
#include "../../source engine/game_events.h"
#include "../../source engine/model_info.h"
#include "../../source engine/material_system_config.h"

namespace hooks {
	bool init();
	bool undo();

	bool __fastcall create_move(c_client_mode* ecx, uintptr_t edx, float frame_time, c_user_cmd* user_cmd);
	int __fastcall do_post_screen_effects(c_client_mode* ecx, uintptr_t edx, int effect_id);
	void __fastcall draw_model_execute(uintptr_t, uintptr_t, void* ctx, void* state, c_model_render_info* info, matrix3x4_t* matrix);
	void __fastcall emit_sound(uintptr_t, uintptr_t, uintptr_t, int, int, const char*, int, const char*, float, float, int, int, int, const vector_t*, const vector_t*, vector_t*, bool, float, int, uintptr_t);
    void __fastcall enable_world_fog();
	bool _fastcall fire_event_client_side(uintptr_t, uintptr_t, c_game_event* event);
	void __fastcall frame_stage_notify(c_base_client_dll* ecx, uintptr_t edx, e_client_frame_stage stage);
	float __fastcall get_screen_aspect_ratio(uintptr_t, uintptr_t, int, int);
	bool __fastcall get_player_info(c_engine_client* ecx, uintptr_t edx, int ent_index, player_info_t& buffer);
	bool __fastcall is_connected(c_engine_client* ecx, uintptr_t edx);
	bool __fastcall is_playing_demo(c_engine_client* ecx, uintptr_t edx);
	void __fastcall level_init_post_entity(c_base_client_dll* ecx, uintptr_t edx);
	void __fastcall level_shutdown_pre_entity(c_base_client_dll* ecx, uintptr_t edx);
    int __fastcall list_leaves_in_box(uintptr_t ecx, uintptr_t edx, const vector_t &mins, const vector_t &maxs, void *leaves_list, int max_leaves);
	void __fastcall lock_cursor(c_vgui_surface* ecx, uintptr_t edx);
	bool __fastcall override_config(uintptr_t, uintptr_t, material_system_config_t*, bool);
	void __fastcall override_mouse_input(c_client_mode* ecx, uintptr_t edx, float* x, float* y);
	int __fastcall override_view(c_client_mode* ecx, uintptr_t edx, view_setup_t* view_setup);
	int __fastcall send_datagram(c_net_channel*, uintptr_t*, void*);
	void __fastcall paint_traverse(c_vgui_panel* ecx, uintptr_t edx, int panel_id, bool force_repaint, bool allow_force);
    bool __fastcall write_user_cmd_delta_to_buffer(uintptr_t, uintptr_t, int slot, bf_write *buf, int from, int to, bool new_user_cmd);

	long __stdcall present(IDirect3DDevice9* device, RECT* source_rect, RECT* dest_rect, HWND dest_window_override, RGNDATA* dirty_region);

	inline decltype(&create_move) create_move_original;
	inline decltype(&do_post_screen_effects) do_post_screen_effects_original;
	inline decltype(&draw_model_execute) draw_model_execute_original;
	inline decltype(&emit_sound) emit_sound_original;
    inline decltype(&enable_world_fog) enable_world_fog_original;
	inline decltype(&fire_event_client_side) fire_event_client_side_original;
	inline decltype(&frame_stage_notify) frame_stage_notify_original;
	inline decltype(&get_screen_aspect_ratio) get_screen_aspect_ratio_original;
	inline decltype(&get_player_info) get_player_info_original;
	inline decltype(&is_connected) is_connected_original;
	inline decltype(&is_playing_demo) is_playing_demo_original;
	inline decltype(&level_init_post_entity) level_init_post_entity_original;
	inline decltype(&level_shutdown_pre_entity) level_shutdown_pre_entity_original;
	inline decltype(&list_leaves_in_box) list_leaves_in_box_original;
	inline decltype(&lock_cursor) lock_cursor_original;
	inline decltype(&override_config) override_config_original;
	inline decltype(&override_mouse_input) override_mouse_input_original;
	inline decltype(&override_view) override_view_original;
	inline decltype(&send_datagram) send_datagram_original;
	inline decltype(&paint_traverse) paint_traverse_original;
    inline decltype(&write_user_cmd_delta_to_buffer) write_user_cmd_delta_to_buffer_original;

	inline decltype(&present) present_original;
}
