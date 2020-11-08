#include <Windows.h>
#include <winternl.h>

#include <array>
#include <cstdio>

#include "../utils/hash.hpp"
#include "../thirdparty/xorstr/xorstr.hpp"
#include "interfaces.hpp"

#include <stdexcept>

struct interface_entry_t
{
  uint32_t module_hash;
  uint32_t interface_hash;

  uintptr_t address;

  constexpr interface_entry_t()
    : module_hash(0), interface_hash(0), address(0) {
  }

  constexpr interface_entry_t(const uint32_t module_hash, const uint32_t interface_hash)
    : module_hash(module_hash), interface_hash(interface_hash), address(0) {
  }
};

__declspec(dllexport) std::array<interface_entry_t, 7> g_interfaces = {
  interface_entry_t(FORCE_CT(hash::fnv("client.dll")), FORCE_CT(hash::fnv("VClient008"))),
  interface_entry_t(FORCE_CT(hash::fnv("client.dll")), FORCE_CT(hash::fnv("VClientEntityList003"))),
  interface_entry_t(FORCE_CT(hash::fnv("engine.dll")), FORCE_CT(hash::fnv("VEngineClient014"))),
  interface_entry_t(FORCE_CT(hash::fnv("inputsystem.dll")), FORCE_CT(hash::fnv("InputSystemVersion001"))),
  interface_entry_t(FORCE_CT(hash::fnv("client.dll")), FORCE_CT(hash::fnv("GameMovement001"))),
  interface_entry_t(FORCE_CT(hash::fnv("engine.dll")), FORCE_CT(hash::fnv("GAMEEVENTSMANAGER002"))),
};

#ifdef _DEBUG
inline uintptr_t find_interface_impl(const uintptr_t module_base, const uint32_t interface_hash) {
  struct interface_reg_t
  {
    uintptr_t (*create_fn)();
    const char* interface_name;
    interface_reg_t* next_ptr;
  };

  const auto create_interface = GetProcAddress(reinterpret_cast<HMODULE>(module_base), XORSTR("CreateInterface"));

  if (create_interface == nullptr)
    return 0;

  // 311C53C0 | 55             | push ebp
  // 311C53C1 | 8B EC          | mov  ebp, esp
  // 311C53C3 | 5D             | pop  ebp
  // 311C53C4 | E9 87 FF FF FF | jmp  311C5350
  const auto jmp_instruction = reinterpret_cast<uintptr_t>(create_interface) + 4;
  const auto jmp_target = jmp_instruction + *reinterpret_cast<size_t*>(jmp_instruction + 1) + 5;
  const auto interface_list = **reinterpret_cast<interface_reg_t***>(jmp_target + 6);

  for (auto list_head = interface_list; list_head != nullptr; list_head = list_head->next_ptr) {
    if (hash::fnv(list_head->interface_name) != interface_hash)
      continue;

    return list_head->create_fn();
  }

  return 0;
}

inline uintptr_t find_interface(const uint32_t module_hash, const uint32_t interface_hash) {
  struct ldr_data_table_entry_t
  {
    PVOID reserved_1[2];
    LIST_ENTRY in_memory_order_links;
    PVOID reserved_2[2];
    PVOID dll_base;
    PVOID reserved_3[2];
    UNICODE_STRING full_dll_name;
    UNICODE_STRING base_dll_name;
  };

  for (auto& iface : g_interfaces) {
    if (iface.module_hash != module_hash || iface.interface_hash != interface_hash)
      continue;

    const auto peb = reinterpret_cast<PEB*>(reinterpret_cast<TEB*>(__readfsdword(0x18))->ProcessEnvironmentBlock);
    const auto module_list = &peb->Ldr->InMemoryOrderModuleList;

    if (iface.address != 0)
      goto GOT_INTERFACE;

    for (auto i = module_list->Flink; i != module_list; i = i->Flink) {
      const auto entry = CONTAINING_RECORD(i, ldr_data_table_entry_t, in_memory_order_links);

      if (!entry)
        continue;

      char name_str[256];

      size_t converted_size = 0;

      memset(name_str, 0, sizeof name_str);
      wcstombs_s(&converted_size, name_str, entry->base_dll_name.Buffer, sizeof name_str);

      for (auto j = 0u; j < converted_size; j++) {
        name_str[j] = static_cast<char>(tolower(name_str[j]));
      }

      if (hash::fnv(name_str) != module_hash)
        continue;

      iface.address = find_interface_impl(reinterpret_cast<uintptr_t>(entry->dll_base), interface_hash);

      goto GOT_INTERFACE;
    }

  GOT_INTERFACE:
    if (iface.address == 0) {
      char buffer[128];

      sprintf_s(buffer, sizeof buffer, XORSTR("Failed to find the interface: %08X %08X \n"), module_hash, interface_hash);

      MessageBoxA(nullptr, buffer, nullptr, MB_OK | MB_ICONERROR);
    }

    return iface.address;
  }

  // @todo: report error

  char buffer[128];

  sprintf_s(buffer, sizeof buffer, XORSTR("Failed to find the interface: %08X %08X \n"), module_hash, interface_hash);

  MessageBoxA(nullptr, buffer, nullptr, MB_OK | MB_ICONERROR);

  return 0;
}
#else
inline uintptr_t find_interface(const uint32_t module_hash, const uint32_t interface_hash) {
  for (const auto& iface : g_interfaces) {
    if (iface.module_hash == module_hash && iface.interface_hash == interface_hash)
      return iface.address;
  }

  // @todo: report error

  char buffer[128];

  sprintf_s(buffer, sizeof buffer, XORSTR("Fatal error during initialization. Please reach out to us via a ticket. %08X %08X \n"), module_hash, interface_hash);

  MessageBoxA(nullptr, buffer, nullptr, MB_OK | MB_ICONERROR);

  return 0;
}
#endif

void interfaces::initialize() {
  client = find_interface(FORCE_CT(hash::fnv("client.dll")), FORCE_CT(hash::fnv("VClient008")));
  entity_list = find_interface(FORCE_CT(hash::fnv("client.dll")), FORCE_CT(hash::fnv("VClientEntityList003")));
  engine_client = find_interface(FORCE_CT(hash::fnv("engine.dll")), FORCE_CT(hash::fnv("VEngineClient014")));
  input_system = find_interface(FORCE_CT(hash::fnv("inputsystem.dll")), FORCE_CT(hash::fnv("InputSystemVersion001")));
  game_movement = find_interface(FORCE_CT(hash::fnv("client.dll")), FORCE_CT(hash::fnv("GameMovement001")));
  event_manager = find_interface(FORCE_CT(hash::fnv("engine.dll")), FORCE_CT(hash::fnv("GAMEEVENTSMANAGER002")));
}
