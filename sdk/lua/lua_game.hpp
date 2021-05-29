#pragma once

#include "lua_internal.hpp"
#include "lua_handler.hpp"

#include "../core/interfaces/interfaces.h"
#include "../source engine/view_setup.h"

namespace lua {
    inline struct {
        CRITICAL_SECTION critical_section;
        bool initialized = false;
    } mutex;

    inline lua_internal::handler handler;

    bool init();
    void reload();

    namespace callbacks {
        void run_events(c_game_event *game_event);
        void run_command(c_user_cmd *cmd);
        void override_view(view_setup_t *view_setup);
        void draw();
    }
}
