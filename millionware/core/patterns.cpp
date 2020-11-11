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

  // TerminateProcess(GetCurrentProcess(), -1);

  return 0;
}

void patterns::initialize() {
#ifdef _DEBUG
  for (auto& entry : g_patterns) {
    const auto module_handle = GetModuleHandleA(entry.module_name);

    if (module_handle != nullptr) {
      const auto dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(module_handle);
      const auto nt_headers = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<uintptr_t>(module_handle) + dos_header->e_lfanew);

      const auto range_start = reinterpret_cast<uintptr_t>(module_handle);
      const auto range_end = range_start + nt_headers->OptionalHeader.SizeOfImage;

      for (auto addr = range_start; addr < range_end; addr++) {
        auto pattern = entry.pattern;
        auto offset = 0u;
        auto found_pattern = true;

        while (*pattern != 0 && found_pattern) {
          if (*pattern == '?') {
            pattern += 2;
          }
          else {
            const auto byte = static_cast<uint8_t>(strtol(pattern, nullptr, 16));

            if (*reinterpret_cast<uint8_t*>(addr + offset) != byte)
              found_pattern = false;

            pattern += 3;
          }

          offset++;
        }

        if (found_pattern) {
          entry.address = addr;

          break;
        }
      }
    }

    entry.module_hash = hash::fnv(entry.module_name);
    entry.pattern_hash = hash::fnv(entry.pattern);

    memset(entry.module_name, 0, sizeof entry.module_name);
    memset(entry.pattern, 0, sizeof entry.pattern);
  }
#endif

  client_state = get_pattern(HASH_FNV_CT("engine.dll"), HASH_FNV_CT("A1 ? ? ? ? 8B 80 ? ? ? ? C3"));
  d3d9_present = get_pattern(HASH_FNV_CT("gameoverlayrenderer.dll"), HASH_FNV_CT("55 8B EC 83 EC 40 B9 ? ? ? ? 53 56"));
  d3d9_reset = get_pattern(HASH_FNV_CT("gameoverlayrenderer.dll"), HASH_FNV_CT("55 8B EC 64 A1 00 00 00 00 B9 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 00 00 00 00"));
  hud_system = get_pattern(HASH_FNV_CT("client.dll"), HASH_FNV_CT("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08"));
  render_beams = get_pattern(HASH_FNV_CT("client.dll"), HASH_FNV_CT("A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 08"));
  weapon_system = get_pattern(HASH_FNV_CT("client.dll"), HASH_FNV_CT("8B 35 ? ? ? ? FF 10 0F B7 C0"));
}
