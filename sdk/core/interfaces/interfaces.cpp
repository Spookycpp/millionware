#include <cstdio>
#include <windows.h>

#include "../../engine/hash/hash.h"
#include "../../engine/logging/logging.h"
#include "../../engine/pe/pe.h"
#include "../../engine/security/xorstr.h"
#include "../exports/exports.h"
#include "../patterns/patterns.h"
#include "interfaces.h"

uint32_t get_interface_internal(std::string_view module_name, std::string_view interface_name)
{
#ifdef _DEBUG
	using create_interface_fn = uint32_t(*)(const char *, int *);

	const auto create_interface = (create_interface_fn) pe::get_export(module_name, XORSTR("CreateInterface"));

	if (!create_interface)
		return 0u;

	return create_interface(interface_name.data(), nullptr);
#else
	char interface_hash_buffer[80];

	sprintf_s(interface_hash_buffer, XORSTR("%s:%s"), module_name.data(), interface_name.data());

	return exports::get_interface(CRC(interface_hash_buffer));
#endif
}

inline uint32_t get_interface(std::string_view module_name, std::string_view interface_name)
{
	const auto result = get_interface_internal(module_name, interface_name);

	if (result != 0u)
	{
		logging::debug(XORSTR("found interface '{}' in '{}' at 0x{:08x}"), interface_name.data(), module_name.data(), result);
	}
	else
	{
		logging::error(XORSTR("couldn't find interface '{}' in '{}'"), interface_name.data(), module_name.data());
	}

	return result;
}

bool interfaces::init()
{
	if ((client_dll = (c_base_client_dll *) get_interface(XORSTR("client.dll"), XORSTR("VClient018"))) == nullptr)
		return false;

	if ((game_movement = (c_game_movement *) get_interface(XORSTR("client.dll"), XORSTR("GameMovement001"))) == nullptr)
		return false;

	if ((entity_list = (c_entity_list *) get_interface(XORSTR("client.dll"), XORSTR("VClientEntityList003"))) == nullptr)
		return false;
	
	if ((effects = (c_effects *) get_interface(XORSTR("client.dll"), XORSTR("IEffects001"))) == nullptr)
		return false;	
	
	if ((engine_sound = (c_engine_sound *) get_interface(XORSTR("engine.dll"), XORSTR("IEngineSoundClient003"))) == nullptr)
		return false;

	if ((input_system = (c_input_system *) get_interface(XORSTR("inputsystem.dll"), XORSTR("InputSystemVersion001"))) == nullptr)
		return false;

	if ((engine_client = (c_engine_client *) get_interface(XORSTR("engine.dll"), XORSTR("VEngineClient014"))) == nullptr)
		return false;

	if ((trace = (c_engine_trace *) get_interface(XORSTR("engine.dll"), XORSTR("EngineTraceClient004"))) == nullptr)
		return false;

	if ((debug_overlay = (c_debug_overlay *) get_interface(XORSTR("engine.dll"), XORSTR("VDebugOverlay004"))) == nullptr)
		return false;	
	
	if ((game_events = (c_game_event_manager2*)get_interface(XORSTR("engine.dll"), XORSTR("GAMEEVENTSMANAGER002"))) == nullptr) {
		return false;
	}
	else {
		_event_listener = std::make_unique< c_event_listener >();
	}

	if ((game_event_manager = (c_game_event_manager*) get_interface(XORSTR("engine.dll"), XORSTR("GAMEEVENTSMANAGER002"))) == nullptr)
		return false;

	if ((panorama = (c_panorama_engine *) get_interface(XORSTR("panorama.dll"), XORSTR("PanoramaUIEngine001"))) == nullptr)
		return false;

	if ((model_info = (c_model_info *) get_interface(XORSTR("engine.dll"), XORSTR("VModelInfoClient004"))) == nullptr)
		return false;	
	
	if ((model_render = (c_model_render *) get_interface(XORSTR("engine.dll"), XORSTR("VEngineModel016"))) == nullptr)
		return false;

	if ((material_system = (c_material_system*) get_interface(XORSTR("materialsystem.dll"), XORSTR("VMaterialSystem080"))) == nullptr)
		return false;

	if ((panel = (c_vgui_panel *) get_interface(XORSTR("vgui2.dll"), XORSTR("VGUI_Panel009"))) == nullptr)
		return false;

	if ((surface = (c_vgui_surface *) get_interface(XORSTR("vguimatsurface.dll"), XORSTR("VGUI_Surface031"))) == nullptr)
		return false;

	if ((convar_system = (c_cvar *) get_interface(XORSTR("vstdlib.dll"), XORSTR("VEngineCvar007"))) == nullptr)
		return false;

	if ((prediction = (c_prediction*) get_interface(XORSTR("client.dll"), XORSTR("VClientPrediction001"))) == nullptr)
		return false;
	
	if ((localize = (c_localize*) get_interface(XORSTR("localize.dll"), XORSTR("Localize_001"))) == nullptr)
		return false;	


	if ((d3d9_device = (IDirect3DDevice9 *) patterns::get_d3d9_device()) == nullptr)
	{
		logging::error(XORSTR("failed to find d3d9 device pattern"));

		return false;
	}

	if ((client_mode = (c_client_mode *) patterns::get_client_mode()) == nullptr)
	{
		logging::error(XORSTR("failed to find client mode pattern"));

		return false;
	}

	if ((global_vars = (c_global_vars *) patterns::get_global_vars()) == nullptr)
	{
		logging::error(XORSTR("failed to find global variables pattern"));

		return false;
	}

	if ((client_state = (c_client_state *) patterns::get_client_state()) == nullptr)
	{
		logging::error(XORSTR("failed to find client state pattern"));

		return false;
	}

	if ((weapon_system = (c_weapon_system *) patterns::get_weapon_system()) == nullptr)
	{
		logging::error(XORSTR("failed to find weapon system pattern"));

		return false;
	}

	if ((input = (c_input *) patterns::get_input()) == nullptr)
	{
		logging::error(XORSTR("failed to find input pattern"));

		return false;
	}

	if ((move_helper = (c_move_helper *) patterns::get_move_helper()) == nullptr) {
        logging::error(XORSTR("failed to find move helper pattern"));

        return false;
    }

	return true;
}
