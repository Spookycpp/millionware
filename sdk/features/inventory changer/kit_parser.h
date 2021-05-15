#pragma once

#include <string>
#include <vector>

namespace kit_parser {
    struct paint_kit_t {
        int id;
        std::string name;

        bool operator<(const paint_kit_t &other) const {
            return name < other.name;
        }
    };

    extern std::vector<paint_kit_t> skin_kits;
    extern std::vector<paint_kit_t> glove_kits;
    extern std::vector<paint_kit_t> sticker_kits;

    extern void initialize_kits();
} // namespace kit_parser