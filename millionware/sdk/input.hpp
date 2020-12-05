#pragma once
#include "vector.hpp"
#include "user_cmd.hpp"

class c_input {
private:
    char padding_0[4];

public:
    bool track_ir;
    bool mouse_init;
    bool mouse_active;
    bool joystick_adv_init;

private:
    char padding_1[164];

public:
    bool camera_intercepting_mouse;
    bool camera_in_third_person;
    bool camera_moving_with_mouse;
    vector3_t camera_offset;
    bool camera_distance_move;
    int camera_old_x;
    int camera_old_y;
    int camera_x;
    int camera_y;
    bool camera_is_orthographic;
    float last_forward_move;
    int clear_input_state;
    user_cmd_t* commands;
};