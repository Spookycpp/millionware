#pragma once

#include "../../source engine/input.h"
#include "../../source engine/trace.h"
#include "../../source engine/vector.h"

namespace features::nade_prediction {
    struct nade_point_t {
        vector_t end_position;
        bool hit_glass;
        bool plane;
        bool detonate;
    };

    void on_paint_traverse();
    void on_create_move(c_user_cmd *user_cmd, c_weapon *weapon);

    void simulate(c_base_grenade *grenade, const vector_t &view);
    void trace_hull(const vector_t &src, const vector_t &end, trace_t &tr, bool *hit_glass = nullptr);
}; // namespace features::nade_prediction