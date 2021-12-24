#pragma once

#include "../../source engine/entity.h"
#include "../../source engine/input.h"

namespace cheat {
    bool init();               // cheat initialization
    bool undo();               // cheat un-initialization
    inline bool panic = false; // used for preventing ensuring the cheat exits without issue

    inline c_player *local_player;   // used for obtaining the local player
    inline c_user_cmd *user_cmd;     // used for obtaining the user commands
    inline vector_t original_angles; // used for storing the original angles

    inline matrix4x4_t view_matrix;

    inline uint32_t unpredicted_flags;       // used for storing unpredicted flags
    inline vector_t unpredicted_velocity;    // used for storing unpredicted velocity
    inline e_move_type unpredicted_movetype; // used for storing unpredicted movetypes
    inline float unpredicted_curtime;        // used for storing unpredicted current time

    inline bool impact_sound;           // used for preventing impact sound
    inline bool landed;                 // used for suit
    inline bool predicting;             // used for preventing land sound spam
    inline bool created_rain = false;   // used for weather sanity
    inline bool round_changed = false;  // used for preventing bomb esp from bugging out
    inline bool reset_killfeed = false; // used for resetting killfeed
    inline bool set_skybox = false;     // used for setting skybox on new map
    inline uintptr_t run_command;       // used for lua run command
    inline int tick_base_shift;         // used for tickbase manipulation
    inline float fov;                   // used for rendered recoil crosshair

    inline std::string notice_text; // used for push_notice()
} // namespace cheat
