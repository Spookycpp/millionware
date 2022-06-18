#include "kit_parser.h"

#include "../../core/interfaces/interfaces.h"
#include "../../core/patterns/patterns.h"

#include "../../engine/logging/logging.h"
#include "../../engine/pe/pe.h"
#include "../../engine/security/xorstr.h"

#include "item_definitions.h"

std::vector<kit_parser::paint_kit_t> kit_parser::skin_kits;
std::vector<kit_parser::paint_kit_t> kit_parser::glove_kits;
std::vector<kit_parser::paint_kit_t> kit_parser::sticker_kits;

class CCStrike15ItemSchema;
class CCStrike15ItemSystem;

template <typename Key, typename value>
struct node_t {
    int previous_id;    // 0x0000
    int next_id;        // 0x0004
    void *_unknown_ptr; // 0x0008
    int _unknown;       // 0x000C
    Key key;            // 0x0010
    value value;        // 0x0014
};

template <typename Key, typename value>
struct head_t {
    node_t<Key, value> *memory; // 0x0000
    int allocation_count;       // 0x0004
    int grow_size;              // 0x0008
    int start_element;          // 0x000C
    int next_available;         // 0x0010
    int _unknown;               // 0x0014
    int last_element;           // 0x0018
};                              // 0x001C

struct string_t {
    char *buffer;  // 0x0000
    int capacity;  // 0x0004
    int grow_size; // 0x0008
    int length;    // 0x000C
};                 // 0x0010

struct paint_kit_data_t {
    int id;                   // 0x0000
    string_t name;            // 0x0004
    string_t description;     // 0x0014
    string_t item_name;       // 0x0024
    string_t material_name;   // 0x0034
    string_t image_inventory; // 0x0044
    char pad_0001[0x8C];      // 0x0054
};                            // 0x00E0

struct sticker_kit_data_t {
    int id;
    int item_rarity;
    string_t name;
    string_t description;
    string_t item_name;
    string_t material_name;
    string_t image_inventory;
    int tournament_event_id;
    int tournament_team_id;
    int tournament_player_id;
    bool is_custom_sticker_material;
    float rotate_end;
    float rotate_start;
    float scale_min;
    float scale_max;
    float wear_min;
    float wear_max;
    string_t image_inventory2;
    string_t image_inventory_large;
    char pad_0001[0x10];
};

void kit_parser::initialize_kits() {

    using usc2_to_utf8_t = int(__cdecl *)(const wchar_t *ucs2, char *utf8, int length);
    static usc2_to_utf8_t usc2_to_utf8_fn = (usc2_to_utf8_t) GetProcAddress(GetModuleHandleA(xs("vstdlib.dll")), xs("V_UCS2ToUTF8"));

    // static auto usc2_to_utf8_fn = pe::get_export<int (*)(const wchar_t *ucs2, char *utf8, int
    // length)>(pe::get_module(XORSTR("vstdlib.dll")), XORSTR("V_UCS2ToUTF8"));

    static auto address = patterns::get_kit_parser_data_1();

    if (!address) {
        logging::info(xs("Failed to get kit parser data (1)"));
        return;
    }

    static auto item_system_fn =
        reinterpret_cast<CCStrike15ItemSystem *(*) ()>(&address + 5 + *reinterpret_cast<uintptr_t *>(&address + 1));
    static auto item_schema = reinterpret_cast<CCStrike15ItemSchema *>(reinterpret_cast<uintptr_t>(item_system_fn()) + sizeof(void *));

    {
        static auto get_paint_kit_definition_offset = *reinterpret_cast<uintptr_t *>(&address + 12);
        static auto get_paint_kit_definition_fn = reinterpret_cast<paint_kit_data_t *(__thiscall *) (CCStrike15ItemSchema *, int)>(
            &address + 16 + get_paint_kit_definition_offset);

        static auto start_element_offset = *reinterpret_cast<intptr_t *>(uintptr_t(get_paint_kit_definition_fn) + 10);
        static auto head_offset = start_element_offset - 12;

        const auto map_head = reinterpret_cast<head_t<int, paint_kit_data_t *> *>(uintptr_t(item_schema) + head_offset);

        if (!map_head) {
            logging::info(xs("Failed to parse paint kits (1)"));
            return;
        }

        for (auto i = 0; i <= map_head->last_element; ++i) {
            const auto paint_kit = map_head->memory[i].value;

            if (paint_kit->id == 9001)
                continue;

            const auto wide_name = interfaces::localize->find(paint_kit->item_name.buffer + 1);

            char name[256] = {0};
            usc2_to_utf8_fn(wide_name, name, sizeof(name));

            if (paint_kit->id < 10000)
                skin_kits.emplace_back(paint_kit_t{paint_kit->id, name});
            else
                glove_kits.emplace_back(paint_kit_t{paint_kit->id, name});
        }

        std::sort(skin_kits.begin(), skin_kits.end());
        std::sort(glove_kits.begin(), glove_kits.end());

        skin_kits.at(0).name = xs("None");
        glove_kits.at(0).name = xs("None");
    }

    {
        address = patterns::get_kit_parser_data_2();

        if (!address) {
            logging::info(xs("Failed to get kit parser data (2)"));
            return;
        }

        (uintptr_t &) address += 4;

        const auto get_sticker_kit_definition_offset = *reinterpret_cast<intptr_t *>(&address + 1);
        const auto get_sticker_kit_definition_fn = reinterpret_cast<sticker_kit_data_t *(__thiscall *) (CCStrike15ItemSchema *, int)>(
            &address + 5 + get_sticker_kit_definition_offset);

        const auto start_element_offset = *reinterpret_cast<intptr_t *>(uintptr_t(get_sticker_kit_definition_fn) + 8 + 2);
        const auto head_offset = start_element_offset - 12;

        const auto map_head = reinterpret_cast<head_t<int, sticker_kit_data_t *> *>(uintptr_t(item_schema) + head_offset);

        if (!map_head) {
            logging::info(xs("Failed to parse paint kits (2)"));
            return;
        }

        for (auto i = 0; i <= map_head->last_element; ++i) {
            const auto sticker_kit = map_head->memory[i].value;

            char sticker_name_if_valve_fucked_up_their_translations[64];

            auto sticker_name_ptr = sticker_kit->item_name.buffer + 1;

            if (strstr(sticker_name_ptr, xs("StickerKit_dhw2014_dignitas"))) {
                strcpy_s(sticker_name_if_valve_fucked_up_their_translations, xs("StickerKit_dhw2014_teamdignitas"));
                strcat_s(sticker_name_if_valve_fucked_up_their_translations, sticker_name_ptr + 27);
                sticker_name_ptr = sticker_name_if_valve_fucked_up_their_translations;
            }

            const auto wide_name = interfaces::localize->find(sticker_name_ptr);

            char name[256] = {0};
            usc2_to_utf8_fn(wide_name, name, sizeof(name));

            sticker_kits.emplace_back(paint_kit_t{sticker_kit->id, name});
        }

        std::sort(sticker_kits.begin(), sticker_kits.end());

        sticker_kits.insert(sticker_kits.begin(), paint_kit_t{0, xs("None")});
    }
}