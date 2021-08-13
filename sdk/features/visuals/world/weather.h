#pragma once

#include "../../../source engine/client_dll.h"

namespace features::visuals::weather {
    void reset_weather(bool cleanup = true);
    void update_weather();

    void do_fog();

} // namespace features::visuals::weather