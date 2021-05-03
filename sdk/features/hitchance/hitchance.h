#pragma once
#include <utility>

#include "../../source engine/entity.h"
#include "../../source engine/vector.h"

namespace features::hit_chance {
    struct hit_chance_data_t {
        std::pair<float, float> random;
        std::pair<float, float> inaccuracy;
        std::pair<float, float> spread;
    };

    void initialize();

    vector_t get_spread_direction(c_weapon *weapon, const vector_t &angles, int seed);

    bool can_hit(c_player *target, c_weapon *weapon, const vector_t &angles, int percentage, int hitbox);
} // namespace features::hit_chance