#include <array>
#include <cstdio>
#include <fmt/format.h>
#include <stdexcept>
#include <windows.h>

#include "../utils/error/error.hpp"
#include "../utils/hash/hash.hpp"
#include "../utils/xorstr/xorstr.hpp"
#include "interfaces.hpp"

struct interface_entry_t {
	char module_name[32];
	char interface_name[48];

	uint32_t module_hash;
	uint32_t interface_hash;

	uintptr_t address;
};

__declspec(dllexport) interface_entry_t g_interfaces[] = {
	{ "client.dll", "VClient018" },
	{ "vstdlib.dll", "VEngineCvar007" },
	{ "engine.dll", "VDebugOverlay004" },
	{ "engine.dll", "VEngineClient014" },
	{ "engine.dll", "IEngineSoundClient003" },
	{ "engine.dll", "VEngineVGui001" },
	{ "client.dll", "VClientEntityList003" },
	{ "inputsystem.dll", "InputSystemVersion001" },
	{ "materialsystem.dll", "VMaterialSystem080" },
	{ "engine.dll", "VEngineModel016" },
	{ "vguimatsurface.dll", "VGUI_Surface031" },
	{ "client.dll", "GameMovement001" },
	{ "engine.dll", "GAMEEVENTSMANAGER002" },
	{ "client.dll", "VClientPrediction001" },
	{ "engine.dll", "EngineTraceClient004" },
	{ "engine.dll", "VModelInfoClient004" },
};

inline uintptr_t get_interface(uint32_t module_hash, uint32_t interface_hash) {
	for (const auto& entry : g_interfaces) {
		if (entry.module_hash != module_hash || entry.interface_hash != interface_hash)
			continue;

		if (entry.address == 0)
			break;

		return entry.address;
	}

	utils::report_error(fmt::format(STR_ENC("Couldn't find an interface: %08X %08X"), module_hash, interface_hash));

	return 0;
}

void interfaces::initialize() {
#ifdef _DEBUG
	using create_interface_t = uintptr_t(*)(const char*, int);

	for (auto& entry : g_interfaces) {
		const auto module_handle = GetModuleHandleA(entry.module_name);

		if (module_handle != nullptr) {
			const auto create_iface = reinterpret_cast<create_interface_t>(GetProcAddress(module_handle, STR_ENC("CreateInterface")));

			if (create_iface != nullptr)
				entry.address = create_iface(entry.interface_name, 0);
		}

		entry.module_hash = FNV(entry.module_name);
		entry.interface_hash = FNV(entry.interface_name);

		memset(entry.module_name, 0, sizeof entry.module_name);
		memset(entry.interface_name, 0, sizeof entry.interface_name);
	}
#endif

	client			= (decltype(client))get_interface(FNV_CT("client.dll"), FNV_CT("VClient018"));
	convar_system	= (decltype(convar_system))get_interface(FNV_CT("vstdlib.dll"), FNV_CT("VEngineCvar007"));
	debug_overlay	= (decltype(debug_overlay))get_interface(FNV_CT("engine.dll"), FNV_CT("VDebugOverlay004"));
	engine_client	= (decltype(engine_client))get_interface(FNV_CT("engine.dll"), FNV_CT("VEngineClient014"));
	engine_sound	= (decltype(engine_sound))get_interface(FNV_CT("engine.dll"), FNV_CT("IEngineSoundClient003"));
	engine_vgui		= (decltype(engine_vgui))get_interface(FNV_CT("engine.dll"), FNV_CT("VEngineVGui001"));
	entity_list		= (decltype(entity_list))get_interface(FNV_CT("client.dll"), FNV_CT("VClientEntityList003"));
	input_system	= (decltype(input_system))get_interface(FNV_CT("inputsystem.dll"), FNV_CT("InputSystemVersion001"));
	material_system = (decltype(material_system))get_interface(FNV_CT("materialsystem.dll"), FNV_CT("VMaterialSystem080"));
	model_render	= (decltype(model_render))get_interface(FNV_CT("engine.dll"), FNV_CT("VEngineModel016"));
	vgui_surface	= (decltype(vgui_surface))get_interface(FNV_CT("vguimatsurface.dll"), FNV_CT("VGUI_Surface031"));
	game_movement	= (decltype(game_movement))get_interface(FNV_CT("client.dll"), FNV_CT("GameMovement001"));
	event_manager	= (decltype(event_manager))get_interface(FNV_CT("engine.dll"), FNV_CT("GAMEEVENTSMANAGER002"));
	prediction		= (decltype(prediction))get_interface(FNV_CT("client.dll"), FNV_CT("VClientPrediction001"));
	engine_trace	= (decltype(engine_trace))get_interface(FNV_CT("engine.dll"), FNV_CT("EngineTraceClient004"));
	model_info		= (decltype(model_info))get_interface(FNV_CT("engine.dll"), FNV_CT("VModelInfoClient004"));

	client_mode		= **reinterpret_cast<c_client_mode***>(patterns::client_mode + 5);
	global_vars		= **reinterpret_cast<c_global_vars_base***>(patterns::global_vars + 1);
	weapon_system	= *reinterpret_cast<c_weapon_system**>(patterns::weapon_system + 2);
	input			= *reinterpret_cast<c_input**>(patterns::input + 1);
}