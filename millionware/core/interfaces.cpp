#include <array>
#include <cstdio>
#include <stdexcept>
#include <Windows.h>
#include <winternl.h>

#include "../thirdparty/xorstr/xorstr.hpp"
#include "../utils/error.hpp"
#include "../utils/hash.hpp"
#include "interfaces.hpp"

struct interface_entry_t
{
  char module_name[32];
  char interface_name[32];

  uint32_t module_hash;
  uint32_t interface_hash;

  uintptr_t address;
};

__declspec(dllexport) interface_entry_t g_interfaces[] = {
  {"client.dll", "VClient018", 0, 0, 0},
  {"engine.dll", "VDebugOverlay004", 0, 0, 0},
  {"client.dll", "VClientEntityList003", 0, 0, 0},
  {"engine.dll", "VEngineClient014", 0, 0, 0},
  {"engine.dll", "VEngineVGui001", 0, 0, 0},
  {"vguimatsurface.dll", "VGUI_Surface031", 0, 0, 0},
  {"inputsystem.dll", "InputSystemVersion001", 0, 0, 0},
  {"client.dll", "GameMovement001", 0, 0, 0},
  {"engine.dll", "GAMEEVENTSMANAGER002", 0, 0, 0},
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
      const auto create_iface = reinterpret_cast<create_interface_t>(GetProcAddress(module_handle, XORSTR("CreateInterface")));

      if (create_iface != nullptr)
        entry.address = create_iface(entry.interface_name, 0);
    }

    entry.module_hash = HASH_FNV(entry.module_name);
    entry.interface_hash = HASH_FNV(entry.interface_name);

    memset(entry.module_name, 0, sizeof entry.module_name);
    memset(entry.interface_name, 0, sizeof entry.interface_name);
  }
#endif

  client = reinterpret_cast<c_base_client_dll*>(get_interface(HASH_FNV_CT("client.dll"), HASH_FNV_CT("VClient018")));
  client_mode = **reinterpret_cast<c_client_mode***>(reinterpret_cast<uintptr_t**>(client)[0][10] + 5);
  debug_overlay = reinterpret_cast<c_debug_overlay*>(get_interface(HASH_FNV_CT("engine.dll"), HASH_FNV_CT("VDebugOverlay004")));
  entity_list = reinterpret_cast<c_entity_list*>(get_interface(HASH_FNV_CT("client.dll"), HASH_FNV_CT("VClientEntityList003")));
  global_vars = **reinterpret_cast<c_global_vars_base***>(reinterpret_cast<uintptr_t**>(client)[0][11] + 10);
  engine_client = reinterpret_cast<c_engine_client*>(get_interface(HASH_FNV_CT("engine.dll"), HASH_FNV_CT("VEngineClient014")));
  engine_vgui = reinterpret_cast<c_engine_vgui*>(get_interface(HASH_FNV_CT("engine.dll"), HASH_FNV_CT("VEngineVGui001")));
  vgui_surface = reinterpret_cast<c_vgui_surface*>(get_interface(HASH_FNV_CT("vguimatsurface.dll"), HASH_FNV_CT("VGUI_Surface031")));
  input_system = reinterpret_cast<c_input_system*>(get_interface(HASH_FNV_CT("inputsystem.dll"), HASH_FNV_CT("InputSystemVersion001")));
  game_movement = get_interface(HASH_FNV_CT("client.dll"), HASH_FNV_CT("GameMovement001"));
  event_manager = get_interface(HASH_FNV_CT("engine.dll"), HASH_FNV_CT("GAMEEVENTSMANAGER002"));
}
