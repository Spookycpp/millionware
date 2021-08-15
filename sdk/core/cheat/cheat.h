#pragma once

#include "../../source engine/entity.h"
#include "../../source engine/input.h"

namespace cheat {
    bool init(); // cheat initialization
    bool undo(); // cheat un-initialization

    inline c_player *local_player;   // used for obtaining the local player
    inline c_user_cmd *user_cmd;     // used for obtaining the user commands
    inline vector_t original_angles; // used for storing the original angles

    inline uint32_t unpredicted_flags;       // used for storing unpredicted flags
    inline vector_t unpredicted_velocity;    // used for storing unpredicted velocity
    inline e_move_type unpredicted_movetype; // used for storing unpredicted movetypes
    inline float unpredicted_curtime;        // used for storing unpredicted current time

    inline bool b_predicting;             // used for preventing land sound spam
    inline bool created_rain = false;     // used for weather sanity
    inline bool disconnect_state = false; // used to stop nightmode sanity
    inline uintptr_t run_command;         // used for lua run command
    inline int tick_base_shift;           // used for tickbase manipulation

    inline std::string notice_text; // used for push_notice()
} // namespace cheat
