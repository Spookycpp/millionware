#pragma once

#include <mutex>

#include "lua_handler.hpp"
#include "lua_internal.hpp"

#include "../core/interfaces/interfaces.h"
#include "../source engine/view_setup.h"

namespace lua {
    /*inline struct {
        CRITICAL_SECTION critical_section;
        bool initialized = false;
    } mutex;*/

    inline std::mutex mutex;

    inline lua_internal::handler handler;

    bool init();
    void reload();
    void unload();

    namespace callbacks {
        void startup();
        void run_events(c_game_event *game_event);
        void run_command(c_user_cmd *cmd);
        void setup_command(c_user_cmd *cmd, bool &send_packet);
        void override_view(view_setup_t *view_setup);
        void draw();
        void draw_front();
    } // namespace callbacks
} // namespace lua
