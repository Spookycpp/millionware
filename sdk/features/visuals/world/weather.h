#pragma once

#include "../../../source engine/client_dll.h"

namespace features::visuals::weather {

    void on_frame_stage_notify(e_client_frame_stage frame_stage);

    void reset_weather(bool cleanup = true);
    void update_weather();

    void do_fog();

} // namespace features::visuals::weather