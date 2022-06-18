#pragma once

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/util/util.h"

#include "../../../source engine/input.h"
#include "../../../source engine/vector.h"

#include <algorithm>
#include <array>
#include <deque>

namespace features::legitbot::lag_comp {
    struct lag_record_t {
        int tick_count = 0;
        vector_t position = vector_t();
        vector_t aimbot_position = vector_t();
        std::array<matrix3x4_t, 128> matrices = {};
        vector_t mins = vector_t();
        vector_t maxs = vector_t();

        bool is_valid() const {
            const float correct = std::clamp(util::get_total_latency(), 0.0f, 0.2f); // todo: account for non valveds
            const float delta = correct - (float(cheat::local_player->get_tick_base()) * interfaces::global_vars->interval_per_tick -
                                           TICKS_TO_TIME(tick_count));

            return std::abs(delta) < 0.2f;
        }
    };

    void on_create_move(c_user_cmd *user_cmd);

    lag_record_t *find_best_record(int ent_idx);
    bool backtrack_entity(c_user_cmd *user_cmd, int ent_idx);
    bool can_backtrack_entity(int ent_idx);
    vector_t get_backtracked_position(int ent_idx);

    void store_records();

    std::deque<lag_record_t> &get_record(int ent_idx);
} // namespace features::legitbot::lag_comp