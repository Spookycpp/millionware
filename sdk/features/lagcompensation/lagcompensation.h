#pragma once
#include "../../source engine/client_dll.h"
#include "../../source engine/matrix.h"
#include "../../source engine/vector.h"

#include <algorithm>
#include <array>
#include <deque>

namespace features::lag_compensation {
    struct render_record_t {
        std::array<matrix3x4_t, 128> matrices = {};
        vector_t origin = vector_t();
        float simulation_time = 0.0f;
        float global_time = 0.0f;
        float valid_time = 0.0f;
    };

    void on_frame_stage_notify(e_client_frame_stage frame_stage);

    void store_visuals();
    bool get_render_record(int idx, matrix3x4_t *out);

    std::deque<render_record_t> &get_raw_render_record(int idx);
} // namespace features::lag_compensation