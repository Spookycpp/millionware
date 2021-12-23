#include "hooks.h"
#include <minhook.h>
#include <unordered_map>

#include "../../engine/hash/hash.h"
#include "../../engine/logging/logging.h"
#include "../../engine/security/xorstr.h"

#include "../cheat/cheat.h"

#include "../interfaces/interfaces.h"

#include "../patterns/patterns.h"

static uintptr_t *get_vtable(uintptr_t instance) {
    return *(uintptr_t **) instance;
}

bool hooks::init() {
    MH_Initialize();

    if (interfaces::engine_client->is_in_game())
        cheat::local_player = (c_player *) interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player());

#define UNDO_HOOK(name) name##_hk.restore();
#define MK_HOOK_ADDR(from, name) name##_hk.make((uintptr_t) (from), (uintptr_t) name);
#define MK_HOOK_VTBL(from, index, name) name##_hk.make(get_vtable((uintptr_t) (from))[index], (uintptr_t) name);

    MK_HOOK_ADDR(*(uint32_t *) (patterns::present + 2), present);
    MK_HOOK_ADDR(*(uint32_t *) (patterns::reset + 9), reset);

    MK_HOOK_ADDR(patterns::calc_view, calc_view);
    MK_HOOK_ADDR(patterns::enable_world_fog, enable_world_fog);
    MK_HOOK_ADDR(patterns::get_color_modulation, get_color_modulation);
    MK_HOOK_ADDR(patterns::is_using_static_prop_debug_modes, is_using_static_prop_debug_modes);
    MK_HOOK_ADDR(patterns::send_datagram, send_datagram);
    MK_HOOK_ADDR(patterns::push_notice, push_notice);
    MK_HOOK_ADDR(patterns::play_step_sound, play_step_sound);

    MK_HOOK_VTBL(interfaces::client_mode, 24, create_move);
    MK_HOOK_VTBL(interfaces::client_mode, 44, do_post_screen_effects);
    MK_HOOK_VTBL(interfaces::model_render, 21, draw_model_execute);
    MK_HOOK_VTBL(interfaces::surface, 28, draw_print_text);
    MK_HOOK_VTBL(interfaces::engine_sound, 5, emit_sound);
    MK_HOOK_VTBL(interfaces::game_events, 9, fire_event_client_side);
    MK_HOOK_VTBL(interfaces::client_dll, 37, frame_stage_notify);
    MK_HOOK_VTBL(interfaces::engine_client, 218, get_demo_playback_parameters);
    MK_HOOK_VTBL(interfaces::engine_client, 101, get_screen_aspect_ratio);
    MK_HOOK_VTBL(interfaces::engine_client, 8, get_player_info);
    MK_HOOK_VTBL(interfaces::engine_client, 27, is_connected);
    MK_HOOK_VTBL(interfaces::engine_client, 82, is_playing_demo);
    MK_HOOK_VTBL(interfaces::client_dll, 6, level_init_post_entity);
    MK_HOOK_VTBL(interfaces::client_dll, 7, level_shutdown_pre_entity);
    MK_HOOK_VTBL(interfaces::engine_client->get_bsp_tree_query(), 6, list_leaves_in_box);
    MK_HOOK_VTBL(interfaces::surface, 67, lock_cursor);
    MK_HOOK_VTBL(interfaces::material_system, 21, override_config);
    MK_HOOK_VTBL(interfaces::client_mode, 23, override_mouse_input);
    MK_HOOK_VTBL(interfaces::client_mode, 18, override_view);
    MK_HOOK_VTBL(interfaces::surface, 116, screen_size_changed);
    MK_HOOK_VTBL(interfaces::vgui_engine, 14, engine_paint);

    cheat::run_command = get_vfunc<uintptr_t>(interfaces::prediction, 19);

    return true;
}

bool hooks::undo() {
    UNDO_HOOK(present);
    UNDO_HOOK(reset);
    UNDO_HOOK(calc_view);
    UNDO_HOOK(enable_world_fog);
    UNDO_HOOK(get_color_modulation);
    UNDO_HOOK(is_using_static_prop_debug_modes);
    UNDO_HOOK(send_datagram);
    UNDO_HOOK(push_notice);
    UNDO_HOOK(play_step_sound);
    UNDO_HOOK(create_move);
    UNDO_HOOK(do_post_screen_effects);
    UNDO_HOOK(draw_model_execute);
    UNDO_HOOK(draw_print_text);
    UNDO_HOOK(emit_sound);
    UNDO_HOOK(fire_event_client_side);
    UNDO_HOOK(frame_stage_notify);
    UNDO_HOOK(get_demo_playback_parameters);
    UNDO_HOOK(get_screen_aspect_ratio);
    UNDO_HOOK(get_player_info);
    UNDO_HOOK(is_connected);
    UNDO_HOOK(is_playing_demo);
    UNDO_HOOK(level_init_post_entity);
    UNDO_HOOK(level_shutdown_pre_entity);
    UNDO_HOOK(list_leaves_in_box);
    UNDO_HOOK(lock_cursor);
    UNDO_HOOK(override_config);
    UNDO_HOOK(override_mouse_input);
    UNDO_HOOK(override_view);
    UNDO_HOOK(screen_size_changed);
    UNDO_HOOK(engine_paint);

    return true;
}
