#pragma once

#include <vector>

#include "../../../source engine/game_events.h"

namespace features::game_events::damage_logs {
    void on_hurt_event(const damage_log_event_data_t &data);

    std::vector<damage_log_data_t> get_damage_logs();
    void clear_damage_logs();
}; // namespace features::game_events::damage_logs