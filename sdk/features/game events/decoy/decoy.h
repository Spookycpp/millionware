#pragma once

namespace features::game_events::decoy {
    void on_decoy_started(const grenade_detonate_data_t &data);
    void reset();
    void draw();
} // namespace features::game_events::decoy