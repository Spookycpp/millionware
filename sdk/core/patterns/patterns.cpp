#include <cstdio>
#include <string_view>
#include <vector>
#include <windows.h>

#include "../../engine/hash/hash.h"
#include "../../engine/logging/logging.h"
#include "../../engine/pe/pe.h"
#include "../../engine/security/xorstr.h"

#include "../exports/exports.h"

#include "patterns.h"

uint32_t scan_module_for_pattern(uint32_t module_base, int module_size, std::string_view pattern) {
    auto byte_list = std::vector<uint8_t>();
    auto mask_list = std::vector<uint8_t>();

    byte_list.reserve(pattern.size());
    mask_list.reserve(pattern.size());

    for (auto pattern_char = pattern.data(); pattern_char[0] != '\x00';) {
        if (pattern_char[0] == '?') {
            byte_list.push_back(0);
            mask_list.push_back(true);

            pattern_char++;
        } else if (isxdigit(pattern_char[0])) {
            const auto lower = (pattern_char[1] >= 'A') ? (pattern_char[1] - 'A' + 10) : (pattern_char[1] - '0');
            const auto upper = (pattern_char[0] >= 'A') ? (pattern_char[0] - 'A' + 10) : (pattern_char[0] - '0');

            byte_list.push_back(upper * 16 + lower);
            mask_list.push_back(false);

            pattern_char += 2;
        } else if (isspace(pattern_char[0])) {
            pattern_char++;
        }
    }

    const auto range_end = module_base + module_size - byte_list.size();
    const auto pattern_size = (int) byte_list.size();

    auto mask_list_data = mask_list.data();
    auto byte_list_data = byte_list.data();

    for (auto address = module_base; address <= range_end; address++) {
        auto found = true;

        for (auto i = 0; found && i < pattern_size; i++) {
            if ((bool) mask_list_data[i])
                continue;

            if (*(uint8_t *) (address + i) == byte_list_data[i])
                continue;

            found = false;
        }

        if (found)
            return address;
    }

    return 0u;
}

uint32_t get_pattern_internal(std::string_view module_name, std::string_view pattern) {
    //#ifdef _DEBUG
    const auto module_base = pe::get_module(module_name);

    if (!module_base)
        return 0u;

    const auto module_start = pe::get_module(module_name);
    const auto dos_headers = (IMAGE_DOS_HEADER *) module_start;
    const auto nt_headers = (IMAGE_NT_HEADERS *) (module_start + dos_headers->e_lfanew);

    return scan_module_for_pattern(module_base, nt_headers->OptionalHeader.SizeOfImage, pattern);
    //#else
    //	char pattern_hash_buffer[240];
    //
    //	sprintf_s(pattern_hash_buffer, XORSTR("%s:%s"), module_name.data(), pattern.data());
    //
    //	return exports::get_pattern(CRC(pattern_hash_buffer));
    //#endif
}

uint32_t patterns::get_pattern(std::string_view module_name, std::string_view pattern) {
    const auto result = get_pattern_internal(module_name, pattern);

    if (result != 0u) {
        logging::debug(xs("found pattern '{}' in {} at 0x{:08x}"), pattern.data(), module_name.data(), result);
    } else {
        logging::debug(xs("couldn't find pattern '{}' in {}"), pattern.data(), module_name.data());
        logging::error(xs("couldn't find pattern, report this on the forum."));
    }

    return result;
}

bool patterns::init() {
    if ((engine_vgui_start_drawing = get_pattern(xs("vguimatsurface.dll"), xs("55 8B EC 83 E4 C0 83 EC 38"))) == 0u)
        return false;

    if ((engine_vgui_finish_drawing = get_pattern(xs("vguimatsurface.dll"), xs("8B 0D ???? 56 C6 05"))) == 0u)
        return false;

    if ((input = get_pattern(xs("client.dll"), xs("B9 ???? F3 0F 11 04 24 FF 50 10"))) == 0u)
        return false;

    if ((local_player = get_pattern(xs("client.dll"), xs("8B 0D ???? 83 FF FF 74 07"))) == 0u)
        return false;

    if ((view_matrix = get_pattern(xs("client.dll"), xs("0F 10 05 ???? 8D 85 ???? B9"))) == 0u)
        return false;

    if ((d3d9_device = get_pattern(xs("shaderapidx9.dll"), xs("A1 ???? 50 8B 08 FF 51 0C"))) == 0u)
        return false;

    if ((set_clantag = get_pattern(xs("engine.dll"), xs("53 56 57 8B DA 8B F9 FF 15"))) == 0u)
        return false;

    if ((client_mode = get_pattern(xs("client.dll"), xs("55 8B EC 8B 0D ???? 8B 01 5D FF 60 ? CC 55 8B EC 83 E4 C0"))) == 0u)
        return false;

    if ((client_state = get_pattern(xs("engine.dll"), xs("A1 ???? 33 D2 6A 00 6A 00 33 C9 89 B0"))) == 0u)
        return false;

    if ((global_vars = get_pattern(xs("client.dll"), xs("A1 ???? 5E 8B 40 10"))) == 0u)
        return false;

    if ((is_demo_or_hltv = get_pattern(xs("client.dll"), xs("84 C0 75 09 38 05"))) == 0u)
        return false;

    if ((money_reveal = get_pattern(xs("client.dll"), xs("84 C0 75 0C 5B"))) == 0u)
        return false;

    if ((insert_into_tree_list_leaves_in_box_call = get_pattern(xs("client.dll"), xs("56 52 FF 50 18"))) == 0u)
        return false;

    if ((weapon_system = get_pattern(xs("client.dll"), xs("8B 35 ???? FF 10 0F B7 C0"))) == 0u)
        return false;

    if ((flash_time = get_pattern(xs("client.dll"), xs("F3 0F 10 86 ???? 0F 2F 40 10 76 30"))) == 0u)
        return false;

    if ((line_goes_through_smoke = get_pattern(xs("client.dll"), xs("55 8B EC 83 EC 08 8B 15 ???? 0F 57 C0"))) == 0u)
        return false;

    if ((move_helper = get_pattern(xs("client.dll"), xs("8B 0D ???? 8B 45 ? 51 8B D4 89 02 8B 01"))) == 0u)
        return false;

    if ((accept_match = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F8 8B 4D 08 BA ???? E8 ???? 85 C0 75 12"))) == 0u)
        return false;

    if ((send_datagram = get_pattern(xs("engine.dll"), xs("55 8B EC 83 E4 F0 B8 ???? E8 ???? 56 57 8B F9 89 7C 24 18"))) == 0u)
        return false;

    if ((load_named_sky = get_pattern(xs("engine.dll"), xs("55 8B EC 81 EC ???? 56 57 8B F9 C7 45"))) == 0u)
        return false;

    if ((set_abs_angles = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"))) == 0u)
        return false;

    if ((sequence_activity = get_pattern(xs("client.dll"), xs("55 8B EC 53 8B 5D 08 56 8B F1 83"))) == 0u)
        return false;

    if ((has_bomb = get_pattern(xs("client.dll"), xs("56 8B F1 85 F6 74 31"))) == 0u)
        return false;

    if ((set_absolute_origin = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8 ???? 8B 7D 08"))) == 0u)
        return false;

    if ((report_player = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F8 83 EC 28 8B 4D 08"))) == 0u)
        return false;

    if ((write_user_command = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D"))) == 0u)
        return false;

    if ((create_econ_item = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D"))) == 0u)
        return false;

    // if ((create_econ_item = get_pattern(XORSTR("client.dll"), XORSTR("C7 45 ????? C7 45 ????? C7 45 ????? C7 45 ????? C7 45 ????? C7 45
    // ????? E8 ???? 83 F8 FF 75 09 8D 45 E4 50 E8 ???? 8D 45 E4 C7 45 ????? 50 C7 45 ????? C7 45 ????? C7 45 ????? C7 45 ????? C7 45 ?????
    // C7 45 ????? E8 ???? 83 F8 FF 75 09 8D 45 E4 50 E8 ???? 8D 45 E4 C7 45 ????? 50 C7 45 ????? C7 45 ????? C7 45 ????? C7 45
    // ????? C7 45 ????? C7 45 ????? E8 ???? 83 F8 FF 75 09 8D 45 E4 50 E8 ???? 8D 45 E4 C7 45 ????? 50 C7 45 ????? C7 45 ????? C7 45 ?????
    // C7 45 ????? C7 45 ????? C7 45 ????? E8 ???? 83 F8 FF 75 09 8D 45 E4 50 E8 ???? 8D 45 E4"))) == 0u) 	return false;

    if ((item_schema = get_pattern(xs("client.dll"), xs("A1 ???? 85 C0 75 53"))) == 0u)
        return false;

    if ((custom_name = get_pattern(xs("client.dll"), xs("E8 ???? 8B 46 78 C1 E8 0A A8 01 74 13 8B 46 34"))) == 0u)
        return false;

    if ((custom_description = get_pattern(xs("client.dll"), xs("E8 ???? 33 DB 39 5E 3C 7E 5E"))) == 0u)
        return false;

    if ((attribute_value = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F8 83 EC 3C 53 8B 5D 08 56 57 6A 00"))) == 0u)
        return false;

    if ((update_equipped_state = get_pattern(xs("client.dll"), xs("55 8B EC 8B 45 08 8B D0 C1 EA 10"))) == 0u)
        return false;

    if ((soc_data = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F0 83 EC 18 56 8B F1 57 8B 86"))) == 0u)
        return false;

    if ((static_data = get_pattern(xs("client.dll"), xs("55 8B EC 51 53 8B D9 8B ????? 56 57 8B ????? 85 FF 74 16"))) == 0u)
        return false;

    if ((econ_item_view = get_pattern(xs("client.dll"), xs("E8 ???? 89 44 24 54"))) == 0u)
        return false;

    if ((kit_parser_data_1 = get_pattern(xs("client.dll"), xs("E8 ???? FF 76 0C 8D 48 04 E8"))) == 0u)
        return false;

    if ((kit_parser_data_2 = get_pattern(xs("client.dll"), xs("53 8D 48 04 E8 ???? 8B 4D 10"))) == 0u)
        return false;

    if ((equip_item_in_loadout = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F8 83 EC 24 83 3D ????? 53 56 57 8B F9"))) == 0u)
        return false;

    if ((find_or_create_reference_econ_item = get_pattern(xs("client.dll"), xs("55 8B EC 51 8B 55 0C 53 56"))) == 0u)
        return false;

    if ((clear_inventory_images = get_pattern(xs("client.dll"), xs("55 8B EC 81 EC ???? 80 3D ????? 56 0F 85 ????"))) == 0u)
        return false;

    if ((remove_item = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F8 51 53 56 57 FF 75 0C 8B F1"))) == 0u)
        return false;

    if ((base_type_cache = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F8 83 EC 1C 0F 10 45 08"))) == 0u)
        return false;

    if ((create_base_type_cache = get_pattern(xs("client.dll"), xs("55 8B EC 51 53 56 8B D9 8D 45 08"))) == 0u)
        return false;

    if ((gc_client_system = get_pattern(xs("client.dll"), xs("8B 0D ???? 6A 00 83 EC 10"))) == 0u)
        return false;

    if ((inventory_item_by_item_id = get_pattern(xs("client.dll"), xs("55 8B EC 8B 55 08 83 EC 10 8B C2"))) == 0u)
        return false;

    if ((add_econ_item = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F8 A1 ???? 83 EC 14 53 56 57 8B F9 8B 08"))) == 0u)
        return false;

    if ((render_beams = get_pattern(xs("client.dll"), xs("A1 ???? 56 8B F1 B9 ???? FF 50 08"))) == 0u)
        return false;

    if ((client_precipitation = get_pattern(
             xs("client.dll"), xs("55 8B EC 51 53 56 57 8B D9 C6 45 FF 01 33 FF 90 83 3C BD ????? 0F 85 ???? A1 ???? 68 ????"))) == 0u)
        return false;

    if ((clear_death_notices = get_pattern(xs("client.dll"), xs("55 8B EC 83 EC 0C 53 56 8B 71 58"))) == 0u)
        return false;

    if ((find_hud_element_addr_1 = get_pattern(xs("client.dll"), xs("B9 ???? E8 ???? 8B 5D 08"))) == 0u)
        return false;

    if ((find_hud_element_addr_2 = get_pattern(xs("client.dll"), xs("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"))) == 0u)
        return false;

    if ((enable_world_fog = get_pattern(xs("client.dll"), xs("55 8B EC 8B 0D ???? 83 EC 0C 8B 01 53"))) == 0u)
        return false;

    if ((game_rules_proxy = get_pattern(xs("client.dll"), xs("A1 ???? 8B 0D ???? 6A 00 68 ???? C6"))) == 0u)
        return false;

    if ((relay_cluster = get_pattern(xs("steamnetworkingsockets.dll"), xs("B8 ???? B9 ???? 0F 43"))) == 0u)
        return false;

    if ((inventory_unlocker = get_pattern(xs("client.dll"), xs("84 C0 75 05 B0 01 5F"))) == 0u)
        return false;

    if ((key_values_system = get_pattern(xs("client.dll"), xs("55 8B EC 51 33 C0 C7 45"))) == 0u)
        return false;

    if ((load_from_buffer = get_pattern(xs("client.dll"), xs("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04"))) == 0u)
        return false;

    if ((find_key = get_pattern(xs("client.dll"), xs("55 8B EC 83 EC 1C 53 8B D9 85 DB"))) == 0u)
        return false;

    if ((set_string = get_pattern(xs("client.dll"), xs("55 8B EC A1 ???? 53 56 57 8B F9 8B 08 8B 01"))) == 0u)
        return false;

    if ((glow_manager = get_pattern(xs("client.dll"), xs("0F 11 05 ???? 83 C8 01"))) == 0u)
        return false;

    if ((push_notice = get_pattern(
             xs("client.dll"),
             xs("55 8B EC 83 E4 F8 B8 ???? E8 ???? 53 8B D9 8B 0D ???? 56 57 81 F9 ???? 75 0C A1 ???? 35 ???? EB 05 8B 01 FF 50 34"))) ==
        0u)
        return false;

    if ((is_breakable = get_pattern(xs("client.dll"), xs("55 8B EC 51 56 8B F1 85 F6 74 68"))) == 0u)
        return false;

    if ((play_step_sound = get_pattern(xs("client.dll"), xs("55 8B EC 8B 45 18 81 EC"))) == 0u)
        return false;

    if ((calc_view = get_pattern(xs("client.dll"), xs("55 8B EC 83 EC 14 53 56 57 FF 75 18"))) == 0u)
        return false;

    if ((get_color_modulation = get_pattern(xs("materialsystem.dll"), xs("55 8B EC 83 EC ? 56 8B F1 8A 46"))) == 0u)
        return false;

    if ((is_using_static_prop_debug_modes = get_pattern(
             xs("engine.dll"), xs("8B 0D ???? 81 F9 ???? 75 ? A1 ???? 35 ???? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ???? 8B 0D"))) == 0u)
        return false;

    if ((smoke_count = get_pattern(xs("client.dll"), xs("A3 ???? 57 8B CB"))) == 0u)
        return false;

    if ((present = get_pattern(xs("gameoverlayrenderer.dll"), xs("FF 15 ???? 8B F0 85 FF"))) == 0u)
        return false;

    if ((reset = get_pattern(xs("gameoverlayrenderer.dll"), xs("C7 45 ????? FF 15 ???? 8B D8"))) == 0u)
        return false;

    return true;
}