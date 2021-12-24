#pragma once

#include <cstdint>
#include <d3d9.h>
#include <memory>

#include "../../engine/hash/hash.h"
#include "../../source engine/client_dll.h"
#include "../../source engine/engine_client.h"
#include "../../source engine/game_events.h"
#include "../../source engine/input.h"
#include "../../source engine/material_system_config.h"
#include "../../source engine/model_info.h"
#include "../../source engine/vgui.h"
#include "../../source engine/view_setup.h"

namespace hooks {
    bool init();
    bool undo();

    void __fastcall calc_view(uintptr_t, uintptr_t, vector_t &eye_origin, vector_t &eye_angles, float &z_near, float &z_far, float &fov);
    bool __fastcall create_move(c_client_mode *ecx, uintptr_t edx, float frame_time, c_user_cmd *user_cmd);
    int __fastcall do_post_screen_effects(c_client_mode *ecx, uintptr_t edx, int effect_id);
    void __fastcall draw_model_execute(uintptr_t, uintptr_t, void *ctx, void *state, c_model_render_info *info, matrix3x4_t *matrix);
    bool __fastcall draw_print_text(uintptr_t, uintptr_t, const wchar_t *text, int text_length, int draw_type);
    void __fastcall emit_sound(uintptr_t, uintptr_t, uintptr_t, int, int, const char *, int, const char *, float, float, int, int, int, const vector_t *, const vector_t *, vector_t *, bool, float, int, uintptr_t);
    void __fastcall enable_world_fog();
    void __fastcall engine_paint(uintptr_t, uintptr_t, int);
    bool _fastcall fire_event_client_side(uintptr_t, uintptr_t, c_game_event *event);
    void __fastcall frame_stage_notify(c_base_client_dll *ecx, uintptr_t edx, e_client_frame_stage stage);
    void __fastcall get_color_modulation(uintptr_t, uintptr_t, float &r, float &g, float &b);
    c_demo_playback_paramaters *__fastcall get_demo_playback_parameters(uintptr_t, uintptr_t);
    float __fastcall get_screen_aspect_ratio(uintptr_t, uintptr_t, int, int);
    bool __fastcall get_player_info(c_engine_client *ecx, uintptr_t edx, int ent_index, player_info_t &buffer);
    bool __fastcall is_connected(c_engine_client *ecx, uintptr_t edx);
    bool __fastcall is_playing_demo(c_engine_client *ecx, uintptr_t edx);
    bool __stdcall is_using_static_prop_debug_modes();
    void __fastcall level_init_post_entity(c_base_client_dll *ecx, uintptr_t edx);
    void __fastcall level_shutdown_pre_entity(c_base_client_dll *ecx, uintptr_t edx);
    int __fastcall list_leaves_in_box(uintptr_t ecx, uintptr_t edx, const vector_t &mins, const vector_t &maxs, void *leaves_list, int max_leaves);
    void __fastcall lock_cursor(c_vgui_surface *ecx, uintptr_t edx);
    bool __fastcall override_config(uintptr_t, uintptr_t, material_system_config_t *, bool);
    void __fastcall override_mouse_input(c_client_mode *ecx, uintptr_t edx, float *x, float *y);
    int __fastcall override_view(c_client_mode *ecx, uintptr_t edx, view_setup_t *view_setup);
    void __fastcall screen_size_changed(uintptr_t, uintptr_t, int, int);
    int __fastcall send_datagram(c_net_channel *, uintptr_t *, void *);
    void __fastcall shutdown(uintptr_t, uintptr_t);
    bool __fastcall write_user_cmd_delta_to_buffer(uintptr_t, uintptr_t, int slot, bf_write *buf, int from, int to, bool new_user_cmd);
    void __fastcall push_notice(uintptr_t, uintptr_t, const char *, int, const char *);
    void __fastcall play_step_sound(c_player *ecx, uintptr_t edx, vector_t &origin, void *surface, float vol, bool force, void *arg);

    long __stdcall reset(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *present_parameters);
    long __stdcall present(IDirect3DDevice9 *device, RECT *source_rect, RECT *dest_rect, HWND dest_window_override, RGNDATA *dirty_region);

    inline decltype(&calc_view) calc_view_original;
    inline decltype(&create_move) create_move_original;
    inline decltype(&do_post_screen_effects) do_post_screen_effects_original;
    inline decltype(&draw_model_execute) draw_model_execute_original;
    inline decltype(&draw_print_text) draw_print_text_original;
    inline decltype(&emit_sound) emit_sound_original;
    inline decltype(&enable_world_fog) enable_world_fog_original;
    inline decltype(&engine_paint) engine_paint_original;
    inline decltype(&fire_event_client_side) fire_event_client_side_original;
    inline decltype(&frame_stage_notify) frame_stage_notify_original;
    inline decltype(&get_color_modulation) get_color_modulation_original;
    inline decltype(&get_demo_playback_parameters) get_demo_playback_parameters_original;
    inline decltype(&get_screen_aspect_ratio) get_screen_aspect_ratio_original;
    inline decltype(&get_player_info) get_player_info_original;
    inline decltype(&is_connected) is_connected_original;
    inline decltype(&is_playing_demo) is_playing_demo_original;
    inline decltype(&is_using_static_prop_debug_modes) is_using_static_prop_debug_modes_original;
    inline decltype(&level_init_post_entity) level_init_post_entity_original;
    inline decltype(&level_shutdown_pre_entity) level_shutdown_pre_entity_original;
    inline decltype(&list_leaves_in_box) list_leaves_in_box_original;
    inline decltype(&lock_cursor) lock_cursor_original;
    inline decltype(&override_config) override_config_original;
    inline decltype(&override_mouse_input) override_mouse_input_original;
    inline decltype(&override_view) override_view_original;
    inline decltype(&screen_size_changed) screen_size_changed_original;
    inline decltype(&send_datagram) send_datagram_original;
    inline decltype(&shutdown) shutdown_original;
    inline decltype(&write_user_cmd_delta_to_buffer) write_user_cmd_delta_to_buffer_original;
    inline decltype(&push_notice) push_notice_original;
    inline decltype(&play_step_sound) play_step_sound_original;

    inline decltype(&reset) reset_original;
    inline decltype(&present) present_original;

} // namespace hooks