#include <array>
#include <cstdio>
#include <stdexcept>
#include <Windows.h>
#include <winternl.h>

#include "../utils/error.hpp"
#include "../utils/hash.hpp"
#include "../utils/xorstr.hpp"
#include "interfaces.hpp"

struct interface_entry_t
{
  uintptr_t* out_interface;

  char module_name[32];
  char interface_name[32];

  uint32_t module_hash;
  uint32_t interface_hash;

  uintptr_t address;
};

__declspec(dllexport) interface_entry_t g_interfaces[] = {
  {reinterpret_cast<uintptr_t*>(&interfaces::client), "client.dll", "VClient018", 0, 0, 0},
  {reinterpret_cast<uintptr_t*>(&interfaces::convar_system), "vstdlib.dll", "VEngineCvar007", 0, 0, 0},
  {reinterpret_cast<uintptr_t*>(&interfaces::debug_overlay), "engine.dll", "VDebugOverlay004", 0, 0, 0},
  {reinterpret_cast<uintptr_t*>(&interfaces::engine_client), "engine.dll", "VEngineClient014", 0, 0, 0},
  {reinterpret_cast<uintptr_t*>(&interfaces::engine_sound), "engine.dll", "IEngineSoundClient003", 0, 0, 0},
  {reinterpret_cast<uintptr_t*>(&interfaces::engine_vgui), "engine.dll", "VEngineVGui001", 0, 0, 0},
  {reinterpret_cast<uintptr_t*>(&interfaces::entity_list), "client.dll", "VClientEntityList003", 0, 0, 0},
  {reinterpret_cast<uintptr_t*>(&interfaces::input_system), "inputsystem.dll", "InputSystemVersion001", 0, 0, 0},
  {reinterpret_cast<uintptr_t*>(&interfaces::material_system), "materialsystem.dll", "VMaterialSystem080", 0, 0, 0},
  {reinterpret_cast<uintptr_t*>(&interfaces::model_render), "engine.dll", "VEngineModel016", 0, 0, 0},
  {reinterpret_cast<uintptr_t*>(&interfaces::vgui_surface), "vguimatsurface.dll", "VGUI_Surface031", 0, 0, 0},
  {reinterpret_cast<uintptr_t*>(&interfaces::game_movement), "client.dll", "GameMovement001", 0, 0, 0},
  {reinterpret_cast<uintptr_t*>(&interfaces::event_manager), "engine.dll", "GAMEEVENTSMANAGER002", 0, 0, 0},
};

inline uintptr_t get_interface(uint32_t module_hash, uint32_t interface_hash) {
  for (const auto& entry : g_interfaces) {
    if (entry.module_hash != module_hash || entry.interface_hash != interface_hash)
      continue;

    if (entry.address == 0)
      break;

    return entry.address;
  }

  utils::error_and_exit(e_error_code::INTERFACES, module_hash, interface_hash);

  return 0;
}

void interfaces::initialize() {
#ifdef _DEBUG
  using create_interface_t = uintptr_t(*)(const char*, int);

  for (auto& entry : g_interfaces) {
    const auto module_handle = GetModuleHandleA(entry.module_name);

    if (module_handle != nullptr) {
      const auto create_iface = reinterpret_cast<create_interface_t>(GetProcAddress(module_handle, XOR("CreateInterface")));

      if (create_iface != nullptr)
        entry.address = create_iface(entry.interface_name, 0);
    }

    entry.module_hash = FNV(entry.module_name);
    entry.interface_hash = FNV(entry.interface_name);

    memset(entry.module_name, 0, sizeof entry.module_name);
    memset(entry.interface_name, 0, sizeof entry.interface_name);
  }
#endif

  for (const auto& entry : g_interfaces) {
    *entry.out_interface = get_interface(entry.module_hash, entry.interface_hash);
  }

  client_mode = **reinterpret_cast<c_client_mode***>(reinterpret_cast<uintptr_t**>(client)[0][10] + 5);
  global_vars = **reinterpret_cast<c_global_vars_base***>(reinterpret_cast<uintptr_t**>(client)[0][11] + 10);
}
