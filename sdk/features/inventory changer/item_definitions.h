#pragma once

#include <vector>

extern bool is_knife(int index);
extern bool is_glove(int index);

namespace item_definitions {
    struct item_info_t {
        constexpr item_info_t(const char *model, const char *icon = nullptr) : model(model), icon(icon) {
        }

        const char *model = nullptr;
        const char *icon = nullptr;
    };

    struct item_name_t {
        constexpr item_name_t(const int id, const char *name) : id(id), name(name) {
        }

        int id = 0;
        const char *name = nullptr;
    };

    struct quality_name_t {
        constexpr quality_name_t(const int id, const char *name) : id(id), name(name) {
        }

        int id = 0;
        const char *name = nullptr;
    };

    struct rarity_name_t {
        constexpr rarity_name_t(const int id, const char *name) : id(id), name(name) {
        }

        int id = 0;
        const char *name = nullptr;
    };

    const item_info_t *get_item_info(int definition_idx);

    extern const std::vector<item_name_t> knife_names;
    extern const std::vector<item_name_t> glove_names;
    extern const std::vector<item_name_t> weapon_names;
    extern const std::vector<quality_name_t> quality_names;
    extern const std::vector<rarity_name_t> rarity_names;
} // namespace item_definitions