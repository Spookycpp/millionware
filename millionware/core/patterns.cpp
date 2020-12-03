#include <cstdio>
#include <cstring>
#include <string>
#include <Windows.h>

#include "../utils/error.hpp"
#include "../utils/hash.hpp"
#include "../utils/xorstr.hpp"
#include "patterns.hpp"

struct pattern_entry_t {
	uintptr_t* out_pattern;

	char module_name[32];
	char pattern[128];

	uint32_t module_hash;
	uint32_t pattern_hash;

	uintptr_t address;
};

__declspec(dllexport) pattern_entry_t g_patterns[] = {
	{&patterns::engine_vgui_start_drawing, "vguimatsurface.dll", "55 8B EC 83 E4 C0 83 EC 38", 0, 0, 0},
	{&patterns::engine_vgui_finish_drawing, "vguimatsurface.dll", "8B 0D ? ? ? ? 56 C6 05", 0, 0, 0},
	{&patterns::player_has_bomb, "client.dll", "56 8B F1 85 F6 74 31", 0, 0, 0},
	{&patterns::set_local_player_ready, "client.dll", "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12", 0, 0, 0},
	{&patterns::set_clantag, "engine.dll", "53 56 57 8B DA 8B F9 FF 15", 0, 0, 0},
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

		entry.module_hash = FNV(entry.module_name);
		entry.pattern_hash = FNV(entry.pattern);

		memset(entry.module_name, 0, sizeof entry.module_name);
		memset(entry.pattern, 0, sizeof entry.pattern);
	}
#endif

	for (const auto& entry : g_patterns) {
		*entry.out_pattern = get_pattern(entry.module_hash, entry.pattern_hash);
	}
}
