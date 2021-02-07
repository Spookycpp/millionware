#pragma once

#include <cstdint>

#include "../sdk/client_dll.hpp"
#include "../sdk/convar_system.hpp"
#include "../sdk/debug_overlay.hpp"
#include "../sdk/engine_client.hpp"
#include "../sdk/engine_vgui.hpp"
#include "../sdk/engine_sound.hpp"
#include "../sdk/entity_list.hpp"
#include "../sdk/global_vars.hpp"
#include "../sdk/input.hpp"
#include "../sdk/input_system.hpp"
#include "../sdk/material_system.hpp"
#include "../sdk/model_render.hpp"
#include "../sdk/vgui.hpp"
#include "../sdk/weapon_system.hpp"
#include "../sdk/client_prediction.hpp"
#include "../sdk/studio.hpp"
#include "../sdk/model_info.hpp"
#include "../sdk/studiobbox.hpp"
#include "../sdk/engine_trace.hpp"
#include "../sdk/engine_render_view.hpp"

namespace interfaces {

	inline c_base_client_dll* client		   = nullptr;
	inline c_client_mode* client_mode		   = nullptr;
	inline c_convar_system* convar_system	   = nullptr;
	inline c_debug_overlay* debug_overlay	   = nullptr;
	inline c_engine_client* engine_client	   = nullptr;
	inline c_engine_sound* engine_sound		   = nullptr;
	inline c_engine_vgui* engine_vgui		   = nullptr;
	inline c_entity_list* entity_list		   = nullptr;
	inline c_global_vars_base* global_vars	   = nullptr;
	inline c_input* input					   = nullptr;
	inline c_input_system* input_system		   = nullptr;
	inline c_material_system* material_system  = nullptr;
	inline c_model_render* model_render		   = nullptr;
	inline c_vgui_surface* vgui_surface		   = nullptr;
	inline c_weapon_system* weapon_system	   = nullptr;
	inline c_prediction* prediction			   = nullptr;
	inline c_game_movement* game_movement	   = nullptr;
	inline i_engine_trace* engine_trace		   = nullptr;
	inline c_model_info* model_info			   = nullptr;
	inline c_engine_render_view* engine_render = nullptr;

	inline uintptr_t event_manager			  = 0;

	void initialize();

}