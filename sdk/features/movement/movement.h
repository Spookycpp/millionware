#pragma once

#include "engine prediction/engine_prediction.h"

#include "../../source engine/client_dll.h"
#include "../../source engine/entity.h"
#include "../../source engine/input.h"
#include "../../source engine/weapon_info.h"

namespace features::movement {

    // eb container
    inline bool predicted_successful = false;
    inline bool should_duck = false;
    inline bool prediction_failed = false;
    inline int mouse_offset = 0;
    inline int prediction_timestamp = 0;
    inline int prediction_ticks = 0;
    inline bool edgebugging = false;
    inline bool edgebugged = false;
    inline vector_t last_known_velocity;

    // prediction container
    inline uint32_t flags;
    inline float fall_velocity;
    inline vector_t origin;
    inline vector_t absolute_origin;
    inline movedata_t movement_data = {};
    inline entity_handle_t ground_ent;

    void pre_prediction(c_user_cmd *user_cmd);
    void post_prediction(c_user_cmd *user_cmd, int pre_flags, int post_flags);

    void predict_edgebug(c_user_cmd *user_cmd);
    void edgebug_assist(c_user_cmd *user_cmd);
    void edgebug_detection(c_user_cmd *user_cmd);
    void slide_walk(c_user_cmd *user_cmd);
    void strafe_optimizer(c_user_cmd *user_cmd, int pre_flags, int post_flags);
    void blockbot(c_user_cmd *user_cmd);
    void autostrafer(c_user_cmd *user_cmd);
} // namespace features::movement