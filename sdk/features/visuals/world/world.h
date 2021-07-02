#pragma once

#include "../../../source engine/client_dll.h"

namespace features::visuals::world {

    void on_frame_stage_notify(e_client_frame_stage frame_stage);

    void nightmode();
    void indicators();
    void velocity_graph();

} // namespace features::visuals::world
