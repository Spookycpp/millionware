#pragma once

#include "../../source engine/game_events.h"

namespace features::game_events {
    void on_player_hurt(c_game_event *game_event);
    void on_item_purchase(const item_purchase_data_t &purchase_data);
    void on_begin_plant(c_game_event *game_event);
    void on_vote_cast(c_game_event *game_event);
    void on_player_death(c_game_event *game_event);
    void on_player_given_c4(c_game_event *game_event);
} // namespace features::game_events