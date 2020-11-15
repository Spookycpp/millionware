#include <cstdio>
#include <cstring>
#include <string>
#include <Windows.h>

#include "../thirdparty/xorstr/xorstr.hpp"
#include "../utils/error.hpp"
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
  {"client.dll", "A1 ? ? ? ? 76 61 F3 0F 10 40 10 0F 2F C1", 0, 0, 0},
  {"vguimatsurface.dll", "55 8B EC 83 E4 C0 83 EC 38", 0, 0, 0},
  {"vguimatsurface.dll", "8B 0D ? ? ? ? 56 C6 05", 0, 0, 0},
  {"client.dll", "56 8B F1 85 F6 74 31", 0, 0, 0},
};

inline uintptr_t get_pattern(uint32_t module_hash, uint32_t pattern_hash) {
  for (const auto& entry : g_patterns) {
    if (entry.module_hash != module_hash || entry.pattern_hash != pattern_hash)
      continue;

    if (entry.address == 0)
      break;

    return entry.address;
  }

  utils::error_and_exit(e_error_code::PATTERNS, module_hash, pattern_hash);

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

    entry.module_hash = HASH_FNV(entry.module_name);
    entry.pattern_hash = HASH_FNV(entry.pattern);

    memset(entry.module_name, 0, sizeof entry.module_name);
    memset(entry.pattern, 0, sizeof entry.pattern);
  }
#endif

  global_vars_base = get_pattern(HASH_FNV_CT("client.dll"), HASH_FNV_CT("A1 ? ? ? ? 76 61 F3 0F 10 40 10 0F 2F C1"));
  engine_vgui_start_drawing = get_pattern(HASH_FNV_CT("vguimatsurface.dll"), HASH_FNV_CT("55 8B EC 83 E4 C0 83 EC 38"));
  engine_vgui_finish_drawing = get_pattern(HASH_FNV_CT("vguimatsurface.dll"), HASH_FNV_CT("8B 0D ? ? ? ? 56 C6 05"));
  player_has_bomb = get_pattern(HASH_FNV_CT("client.dll"), HASH_FNV_CT("56 8B F1 85 F6 74 31"));
}
