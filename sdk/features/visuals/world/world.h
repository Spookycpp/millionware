#pragma once

#include <string>
#include <vector>

#include "../../../source engine/client_dll.h"
#include "../../../source engine/entity.h"

namespace features::visuals::world {
    void indicators();
    void display_spectators();
    void draw_world(c_entity *entity);
    void bomb_timer(c_entity *entity);
} // namespace features::visuals::world
