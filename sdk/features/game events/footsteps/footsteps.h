#pragma once

#include "../../../source engine/client_dll.h"
#include "../../../source engine/game_events.h"

namespace features::game_events::footsteps {
    void on_step_event(c_game_event *game_event);
    void on_paint();
}; // namespace features::game_events::footsteps