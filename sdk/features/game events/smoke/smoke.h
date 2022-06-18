#pragma once

namespace features::game_events::smoke {
    void on_smokegrenade_detonate(const grenade_detonate_data_t &data);
    void reset();
    void draw();
} // namespace features::game_events::smoke