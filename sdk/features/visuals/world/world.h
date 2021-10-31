#pragma once

#include <string>
#include <vector>

#include "../../../source engine/client_dll.h"
#include "../../../source engine/entity.h"

namespace features::visuals::world {

    void on_frame_stage_notify(e_client_frame_stage frame_stage);

    void indicators();

    void display_spectators();

    void draw_world(c_entity *entity);
    void draw_bomb_text(c_entity *entity);
    void bomb_timer(c_entity *entity);
} // namespace features::visuals::world
