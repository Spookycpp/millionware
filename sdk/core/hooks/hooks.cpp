#include <minhook.h>
#include <unordered_map>

#include "../../engine/hash/hash.h"
#include "../../engine/logging/logging.h"
#include "../../engine/security/xorstr.h"
#include "../cheat/cheat.h"
#include "../interfaces/interfaces.h"
#include "../patterns/patterns.h"
#include "hooks.h"

uintptr_t create_hook(uintptr_t from, uintptr_t to)
{
	uintptr_t original;

	if (const auto status = MH_CreateHook((LPVOID) from, (LPVOID) to, (LPVOID *) &original); status != MH_OK)
		return 0;

	return original;
}

template <typename F>
F create_hook(uintptr_t instance, int index, F to)
{
	const auto vtable = *(uintptr_t **) instance;

	return (F) create_hook(vtable[index], (uintptr_t) to);
}

bool hooks::init()
{
	MH_Initialize();

	const auto bsp_tree_query = interfaces::engine_client->get_bsp_tree_query();

	if (interfaces::engine_client->is_in_game())
		cheat::local_player = (c_player *) interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player());

	create_move_original = create_hook((uintptr_t) interfaces::client_mode, 24, &create_move);
	do_post_screen_effects_original = create_hook((uintptr_t) interfaces::client_mode, 44, &do_post_screen_effects);
	draw_model_execute_original = create_hook((uintptr_t) interfaces::model_render, 21, &draw_model_execute);
	emit_sound_original = create_hook((uintptr_t) interfaces::engine_sound, 5, &emit_sound);
	fire_event_client_side_original = create_hook((uintptr_t) interfaces::game_event_manager, 9, &fire_event_client_side);
	frame_stage_notify_original = create_hook((uintptr_t) interfaces::client_dll, 37, &frame_stage_notify);
	get_screen_aspect_ratio_original = create_hook((uintptr_t) interfaces::engine_client, 101, &get_screen_aspect_ratio);
	get_player_info_original = create_hook((uintptr_t) interfaces::engine_client, 8, &get_player_info);
	is_connected_original = create_hook((uintptr_t) interfaces::engine_client, 27, &is_connected);
	is_playing_demo_original = create_hook((uintptr_t) interfaces::engine_client, 82, &is_playing_demo);
	level_init_post_entity_original = create_hook((uintptr_t) interfaces::client_dll, 6, &level_init_post_entity);
	level_shutdown_pre_entity_original = create_hook((uintptr_t) interfaces::client_dll, 7, &level_shutdown_pre_entity);
	list_leaves_in_box_original = create_hook((uintptr_t) bsp_tree_query, 6, &list_leaves_in_box);
	lock_cursor_original = create_hook((uintptr_t) interfaces::surface, 67, &lock_cursor);
	override_mouse_input_original = create_hook((uintptr_t) interfaces::client_mode, 23, &override_mouse_input);
	override_view_original = create_hook((uintptr_t) interfaces::client_mode, 18, &override_view);
	paint_traverse_original = create_hook((uintptr_t) interfaces::panel, 42, &paint_traverse);

	present_original = create_hook((uintptr_t) interfaces::d3d9_device, 17, &present);

#define INIT_HOOK(h, n)											\
	if (h == 0)													\
	{															\
		logging::error(XORSTR("failed to initialize hook " n));	\
		return false;											\
	}

	INIT_HOOK(create_move_original, "CreateMove");
	INIT_HOOK(do_post_screen_effects_original, "DoPostScreenEffects");
	INIT_HOOK(draw_model_execute_original, "DrawModelExecute");
	INIT_HOOK(emit_sound_original, "EmitSound");
	INIT_HOOK(fire_event_client_side_original, "FireEventClientSide");
	INIT_HOOK(frame_stage_notify_original, "FrameStageNotify");
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
	INIT_HOOK(paint_traverse_original, "PaintTraverse");

	INIT_HOOK(present_original, "Present");

	MH_EnableHook(MH_ALL_HOOKS);

	return true;
}

bool hooks::undo()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);

	return true;
}