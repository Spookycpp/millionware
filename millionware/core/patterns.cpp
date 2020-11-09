#include <Windows.h>

#include <cstdio>
#include <cstring>
#include <string_view>

#include "../thirdparty/xorstr/xorstr.hpp"
#include "../utils/hash.hpp"
#include "patterns.hpp"

struct pattern_entry_t
{
  char module_name[32];
  char pattern[128];

  uint32_t module_hash;
  uint32_t pattern_hash;

  uintptr_t address;
};

__declspec(dllexport) pattern_entry_t g_patterns[] = {
  {"engine.dll", "A1 ? ? ? ? 8B 80 ? ? ? ? C3", 0, 0, 0},
  {"gameoverlayrenderer.dll", "55 8B EC 83 EC 40 B9 ? ? ? ? 53 56", 0, 0, 0},
  {"gameoverlayrenderer.dll", "55 8B EC 64 A1 00 00 00 00 B9 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 00 00 00 00", 0, 0, 0},
  {"client.dll", "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08", 0, 0, 0},
  {"client.dll", "A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 08", 0, 0, 0},
  {"client.dll", "8B 35 ? ? ? ? FF 10 0F B7 C0", 0, 0, 0},
};

inline uintptr_t get_pattern(const uint32_t module_hash, const uint32_t pattern_hash) {
  for (const auto& entry : g_patterns) {
    if (entry.module_hash != module_hash || entry.pattern_hash != pattern_hash)
      continue;

    if (entry.address == 0)
      break;

    return entry.address;
  }

  char buffer[128];

  sprintf_s(buffer, sizeof buffer, XORSTR(
              "Fatal error during initialization. \n"
              "Please reach out to us via a ticket. \n"
              "Additional info: E02 %08X %08X"
            ), module_hash, pattern_hash);

  MessageBoxA(nullptr, buffer, nullptr, MB_OK | MB_ICONERROR);
  TerminateProcess(GetCurrentProcess(), -1);

  return 0;
}

void patterns::initialize() {
#ifdef _DEBUG
  // @todo: initialize patterns
#endif

  client_state = get_pattern(HASH_FNV_CT("engine.dll"), HASH_FNV_CT("A1 ? ? ? ? 8B 80 ? ? ? ? C3"));
  d3d9_present = get_pattern(HASH_FNV_CT("gameoverlayrenderer.dll"), HASH_FNV_CT("55 8B EC 83 EC 40 B9 ? ? ? ? 53 56"));
  d3d9_reset = get_pattern(HASH_FNV_CT("gameoverlayrenderer.dll"), HASH_FNV_CT("55 8B EC 64 A1 00 00 00 00 B9 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 00 00 00 00"));
  hud_system = get_pattern(HASH_FNV_CT("client.dll"), HASH_FNV_CT("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08"));
  render_beams = get_pattern(HASH_FNV_CT("client.dll"), HASH_FNV_CT("A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 08"));
  weapon_system = get_pattern(HASH_FNV_CT("client.dll"), HASH_FNV_CT("8B 35 ? ? ? ? FF 10 0F B7 C0"));
}
