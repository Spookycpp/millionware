#include "hooks.h"
#include <minhook.h>
#include <unordered_map>

#include "../../engine/hash/hash.h"
#include "../../engine/logging/logging.h"
#include "../../engine/security/xorstr.h"

#include "../cheat/cheat.h"

#include "../interfaces/interfaces.h"

#include "../patterns/patterns.h"

uintptr_t create_hook(uintptr_t from, uintptr_t to) {
    uintptr_t original;

    if (const auto status = MH_CreateHook((LPVOID) from, (LPVOID) to, (LPVOID *) &original); status != MH_OK)
        return 0;

    return original;
}

static uintptr_t *get_vtable(uintptr_t instance) {
    return *(uintptr_t **) instance;
}

template <typename F>
F create_hook(uintptr_t instance, int index, F to) {
    const auto vtable = get_vtable(instance);

    return (F) create_hook(vtable[index], (uintptr_t) to);
}

bool hooks::init() {
    MH_Initialize();

    if (interfaces::engine_client->is_in_game())
        cheat::local_player = (c_player *) interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player());

#define MAKE_HOOK_ADDR(from, to) to##_original = (decltype(&to)) create_hook((uintptr_t) from, (uintptr_t) &to);
#define MAKE_HOOK_INDEX(inst, index, to) to##_original = (decltype(&to)) create_hook((uintptr_t) inst, index, (uintptr_t) &to);

    MAKE_HOOK_ADDR(patterns::calc_view, calc_view);
    MAKE_HOOK_ADDR(patterns::enable_world_fog, enable_world_fog);
    MAKE_HOOK_ADDR(patterns::get_color_modulation, get_color_modulation);
    MAKE_HOOK_ADDR(patterns::is_using_static_prop_debug_modes, is_using_static_prop_debug_modes);
    MAKE_HOOK_ADDR(patterns::send_datagram, send_datagram);
    MAKE_HOOK_ADDR(patterns::push_notice, push_notice);
    MAKE_HOOK_ADDR(patterns::play_step_sound, play_step_sound);

    MAKE_HOOK_INDEX(interfaces::client_mode, 24, create_move);
    MAKE_HOOK_INDEX(interfaces::client_mode, 44, do_post_screen_effects);
    MAKE_HOOK_INDEX(interfaces::model_render, 21, draw_model_execute);
    MAKE_HOOK_INDEX(interfaces::surface, 28, draw_print_text);
    MAKE_HOOK_INDEX(interfaces::engine_sound, 5, emit_sound);
    MAKE_HOOK_INDEX(interfaces::game_events, 9, fire_event_client_side);
    MAKE_HOOK_INDEX(interfaces::client_dll, 37, frame_stage_notify);
    MAKE_HOOK_INDEX(interfaces::engine_client, 218, get_demo_playback_parameters);
    MAKE_HOOK_INDEX(interfaces::engine_client, 101, get_screen_aspect_ratio);
    MAKE_HOOK_INDEX(interfaces::engine_client, 8, get_player_info);
    MAKE_HOOK_INDEX(interfaces::engine_client, 27, is_connected);
    MAKE_HOOK_INDEX(interfaces::engine_client, 82, is_playing_demo);
    MAKE_HOOK_INDEX(interfaces::client_dll, 4, shutdown);
    MAKE_HOOK_INDEX(interfaces::client_dll, 6, level_init_post_entity);
    MAKE_HOOK_INDEX(interfaces::client_dll, 7, level_shutdown_pre_entity);
    MAKE_HOOK_INDEX(interfaces::engine_client->get_bsp_tree_query(), 6, list_leaves_in_box);
    MAKE_HOOK_INDEX(interfaces::surface, 67, lock_cursor);
    MAKE_HOOK_INDEX(interfaces::material_system, 21, override_config);
    MAKE_HOOK_INDEX(interfaces::client_mode, 23, override_mouse_input);
    MAKE_HOOK_INDEX(interfaces::client_mode, 18, override_view);
    MAKE_HOOK_INDEX(interfaces::surface, 116, screen_size_changed);
    MAKE_HOOK_INDEX(interfaces::vgui_engine, 14, engine_paint);

#undef MAKE_HOOK_ADDR
#undef MAKE_HOOK_INDEX

    std::uintptr_t present_addr = (patterns::get_present() + 2);
    std::uintptr_t reset_addr = (patterns::get_reset() + 9);

    present_original = **reinterpret_cast<decltype(&present_original) *>(present_addr);
    reset_original = **reinterpret_cast<decltype(&reset_original) *>(reset_addr);

    **reinterpret_cast<void ***>(present_addr) = reinterpret_cast<void *>(&present);
    **reinterpret_cast<void ***>(reset_addr) = reinterpret_cast<void *>(&reset);

#define INIT_HOOK(h, n)                                                                                                                    \
    if (h == 0) {                                                                                                                          \
        logging::error(xs("failed to initialize hook " n));                                                                                \
        return false;                                                                                                                      \
    }

    INIT_HOOK(calc_view_original, "Calculate view");
    INIT_HOOK(create_move_original, "CreateMove");
    INIT_HOOK(do_post_screen_effects_original, "DoPostScreenEffects");
    INIT_HOOK(draw_model_execute_original, "DrawModelExecute");
    INIT_HOOK(draw_print_text_original, "DrawPrintText");
    INIT_HOOK(emit_sound_original, "EmitSound");
    INIT_HOOK(enable_world_fog_original, "EnableWorldFog");
    INIT_HOOK(fire_event_client_side_original, "FireEventClientSide");
    INIT_HOOK(frame_stage_notify_original, "FrameStageNotify");
    INIT_HOOK(get_demo_playback_parameters_original, "GetDemoPlayParameters");
    INIT_HOOK(get_screen_aspect_ratio_original, "GetScreenAspectRatioOriginal");
    INIT_HOOK(get_player_info_original, "GetPlayerInfo");
    INIT_HOOK(is_connected_original, "IsConnected");
    INIT_HOOK(is_playing_demo_original, "IsPlayingDemo");
    INIT_HOOK(level_init_post_entity_original, "LevelInitPostEntry");
    INIT_HOOK(level_shutdown_pre_entity_original, "LevelShutdownPreEntity");
    INIT_HOOK(list_leaves_in_box_original, "ListLeavesInBox");
    INIT_HOOK(lock_cursor_original, "LockCursor");
    INIT_HOOK(override_mouse_input_original, "OverrideMouseInput");
    INIT_HOOK(override_view_original, "OverrideView");
    INIT_HOOK(screen_size_changed_original, "ScreenSizeChanged");
    INIT_HOOK(send_datagram_original, "SendDatagram");
    INIT_HOOK(shutdown_original, "Shutdown");
    INIT_HOOK(engine_paint_original, "EnginePaint");
    INIT_HOOK(push_notice_original, "PushNotice");
    INIT_HOOK(play_step_sound_original, "PlayStepSound");

    INIT_HOOK(present_original, "Present");
    INIT_HOOK(reset_original, "Reset");

    MH_EnableHook(MH_ALL_HOOKS);

    return true;
}

bool hooks::undo() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);

    std::uintptr_t present_addr = (patterns::get_present() + 2);
    std::uintptr_t reset_addr = (patterns::get_reset() + 9);

    **reinterpret_cast<void ***>(present_addr) = reinterpret_cast<void *>(present_original);
    **reinterpret_cast<void ***>(reset_addr) = reinterpret_cast<void *>(reset_original);

    return true;
}