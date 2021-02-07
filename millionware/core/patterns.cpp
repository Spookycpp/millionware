#include <cstdio>
#include <cstring>
#include <fmt/format.h>
#include <string>
#include <windows.h>

#include "../utils/error/error.hpp"
#include "../utils/hash/hash.hpp"
#include "../utils/xorstr/xorstr.hpp"
#include "patterns.hpp"

struct pattern_entry_t {
	char module_name[32];
	char pattern[128];

	uint32_t module_hash;
	uint32_t pattern_hash;

	uintptr_t address;
};

__declspec(dllexport) pattern_entry_t g_patterns[] = {
	{ "vguimatsurface.dll", "55 8B EC 83 E4 C0 83 EC 38" },
	{ "vguimatsurface.dll", "8B 0D ? ? ? ? 56 C6 05" },
	{ "client.dll", "56 8B F1 85 F6 74 31" },
	{ "client.dll", "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12" },
	{ "engine.dll", "53 56 57 8B DA 8B F9 FF 15" },
	{ "client.dll", "8B 35 ? ? ? ? FF 10 0F B7 C0" },
	{ "client.dll", "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10" },
	{ "client.dll", "84 C0 75 09 38 05" },
	{ "client.dll", "84 C0 75 0C 5B" },
	{ "client.dll", "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01" },
	{ "client.dll", "A1 ? ? ? ? 5E 8B 40 10" },
	{ "client.dll", "55 8B EC 8B 0D ? ? ? ? 8B 01 5D FF 60 ? CC 55 8B EC 83 E4 C0" },
	{ "client.dll", "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0" },
	{ "client.dll", "F3 0F 10 86 ? ? ? ? 0F 2F 40 10 76 30" },
	{ "engine.dll", "53 56 57 8B DA 8B F9 FF 15" },
};

inline uintptr_t get_pattern(uint32_t module_hash, uint32_t pattern_hash) {
	for (const auto& entry : g_patterns) {
		if (entry.module_hash != module_hash || entry.pattern_hash != pattern_hash)
			continue;

		if (entry.address == 0)
			break;

		return entry.address;
	}

	utils::report_error(fmt::format(STR_ENC("Couldn't find a pattern: %08X %08X"), module_hash, pattern_hash));

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

	engine_vgui_start_drawing = get_pattern(FNV_CT("vguimatsurface.dll"), FNV_CT("55 8B EC 83 E4 C0 83 EC 38"));
	engine_vgui_finish_drawing = get_pattern(FNV_CT("vguimatsurface.dll"), FNV_CT("8B 0D ? ? ? ? 56 C6 05"));
	player_has_bomb = get_pattern(FNV_CT("client.dll"), FNV_CT("56 8B F1 85 F6 74 31"));
	set_local_player_ready = get_pattern(FNV_CT("client.dll"), FNV_CT("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"));
	set_clantag = get_pattern(FNV_CT("engine.dll"), FNV_CT("53 56 57 8B DA 8B F9 FF 15"));
	weapon_system = get_pattern(FNV_CT("client.dll"), FNV_CT("8B 35 ? ? ? ? FF 10 0F B7 C0"));
	input = get_pattern(FNV_CT("client.dll"), FNV_CT("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10"));
	demo_or_hltv = get_pattern(FNV_CT("client.dll"), FNV_CT("84 C0 75 09 38 05"));
	money_reveal = get_pattern(FNV_CT("client.dll"), FNV_CT("84 C0 75 0C 5B"));
	move_helper = get_pattern(FNV_CT("client.dll"), FNV_CT("8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01"));
	global_vars = get_pattern(FNV_CT("client.dll"), FNV_CT("A1 ? ? ? ? 5E 8B 40 10"));
	client_mode = get_pattern(FNV_CT("client.dll"), FNV_CT("55 8B EC 8B 0D ? ? ? ? 8B 01 5D FF 60 ? CC 55 8B EC 83 E4 C0"));
	line_goes_through_smoke = get_pattern(FNV_CT("client.dll"), FNV_CT("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"));
	flashbang_time = get_pattern(FNV_CT("client.dll"), FNV_CT("F3 0F 10 86 ? ? ? ? 0F 2F 40 10 76 30"));
	clan_tag = get_pattern(FNV_CT("engine.dll"), FNV_CT("53 56 57 8B DA 8B F9 FF 15"));
}