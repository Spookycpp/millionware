#pragma once
#include <tuple>

#include "../../source engine/entity.h"
#include "../../source engine/input.h"
#include "../../source engine/vector.h"

namespace features::legitbot {
    struct aim_record_t {
        vector_t view_angles;
        float time;
    };

    using target_t = std::tuple<int, vector_t, int>;

    void on_override_mouse_input(float *x, float *y);
    void on_create_move(c_user_cmd *user_cmd, c_weapon *weapon);

    void aimbot(float *x, float *y);
    void assist(c_player *target, float *x, float *y);
    void flick_bot(c_user_cmd *user_cmd, c_weapon *weapon);
    void standalone_rcs(c_user_cmd *user_cmd, c_weapon *weapon);

    bool update_settings(c_weapon *weapon);

    void aim_at_target(const target_t &data, float *x, float *y);
    void flick_to_target(const target_t &data, c_user_cmd *user_cmd, c_weapon *weapon, bool silent);

    target_t get_target(int method, float fov);
    int get_bone(c_player *target, int method, float fov);

    bool should_activate(bool check_hotkey_only = false);
    bool is_valid_target(c_player *target);

    void sample_angle_data(const vector_t &angles);
    float get_average_delta();

    point_t angles_to_pixels(const vector_t &angles);
    vector_t pixels_to_angles(const point_t &pixels);
} // namespace features::legitbot