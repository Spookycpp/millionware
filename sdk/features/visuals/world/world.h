#pragma once

#include <vector>
#include <string>

#include "../../../source engine/client_dll.h"
#include "../../../source engine/entity.h"

namespace features::visuals::world {

    void on_frame_stage_notify(e_client_frame_stage frame_stage);

    void nightmode();
    void indicators();
    void velocity_graph();

    void display_spectators();

} // namespace features::visuals::world
