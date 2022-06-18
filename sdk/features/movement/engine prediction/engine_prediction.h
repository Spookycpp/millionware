#pragma once

#include "../../../core/interfaces/interfaces.h"
#include "../../../source engine/input.h"

#include <deque>
#include <functional>

namespace features::engine_prediction {
    inline float old_curtime, old_frametime, old_tickcount;
    inline int old_tickbase;
    inline bool old_in_prediction, old_is_first_time_predicted;

    inline int correct_tickbase;
    inline c_user_cmd *last_command;

    void start_prediction();
    void repredict();
    void store();
    void create_edgebug_entry(c_user_cmd *user_cmd);
    void apply_edgebug_data(c_user_cmd *user_cmd);
    void apply_edgebug_flags();
    void end_prediction();

    void run_in_prediction(std::function<void()> fn);

} // namespace features::engine_prediction