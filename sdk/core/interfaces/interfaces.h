#pragma once

#include <d3d9.h>

#include "../../source engine/client_class.h"
#include "../../source engine/client_dll.h"
#include "../../source engine/client_prediction.h"
#include "../../source engine/cvar.h"
#include "../../source engine/debug_overlay.h"
#include "../../source engine/engine_client.h"
#include "../../source engine/engine_sound.h"
#include "../../source engine/entity.h"
#include "../../source engine/input.h"
#include "../../source engine/model_info.h"
#include "../../source engine/model_render.h"
#include "../../source engine/material_system.h"
#include "../../source engine/game_events_manager.h"
#include "../../source engine/globals.h"
#include "../../source engine/panorama.h"
#include "../../source engine/recv_props.h"
#include "../../source engine/trace.h"
#include "../../source engine/vgui.h"
#include "../../source engine/weapon_system.h"

namespace interfaces
{
	inline c_base_client_dll *client_dll;
	inline c_game_movement *game_movement;
	inline c_entity_list *entity_list;
	inline c_engine_sound *engine_sound;
	inline c_engine_client *engine_client;
	inline c_engine_trace *trace;
	inline c_debug_overlay* debug_overlay;
	inline c_input_system *input_system;
	inline c_game_event_manager* game_event_manager;
	inline c_panorama_engine *panorama;
	inline c_model_info *model_info;
	inline c_model_render *model_render;
	inline c_material_system *material_system;
	inline c_vgui_panel *panel;
	inline c_vgui_surface *surface;
	inline c_ui_engine *ui_engine;
	inline c_cvar *convar_system;
	inline c_prediction* prediction;

	inline IDirect3DDevice9 *d3d9_device;
	inline c_client_mode *client_mode;
	inline c_global_vars *global_vars;
	inline c_client_state *client_state;
	inline c_weapon_system *weapon_system;
	inline c_input *input;

	bool init();
}