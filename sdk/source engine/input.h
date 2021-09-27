#pragma once

#include <cstdint>

#include "macros.h"
#include "vector.h"

enum e_buttons {
    BUTTON_IN_ATTACK = 1 << 0,
    BUTTON_IN_JUMP = 1 << 1,
    BUTTON_IN_DUCK = 1 << 2,
    BUTTON_IN_FORWARD = 1 << 3,
    BUTTON_IN_BACK = 1 << 4,
    BUTTON_IN_USE = 1 << 5,
    BUTTON_IN_CANCEL = 1 << 6,
    BUTTON_IN_LEFT = 1 << 7,
    BUTTON_IN_RIGHT = 1 << 8,
    BUTTON_IN_MOVE_LEFT = 1 << 9,
    BUTTON_IN_MOVE_RIGHT = 1 << 10,
    BUTTON_IN_ATTACK2 = 1 << 11,
    BUTTON_IN_RUN = 1 << 12,
    BUTTON_IN_RELOAD = 1 << 13,
    BUTTON_IN_ALT1 = 1 << 14,
    BUTTON_IN_ALT2 = 1 << 15,
    BUTTON_IN_SCORE = 1 << 16,
    BUTTON_IN_SPEED = 1 << 17,
    BUTTON_IN_WALK = 1 << 18,
    BUTTON_IN_ZOOM = 1 << 19,
    BUTTON_IN_WEAPON1 = 1 << 20,
    BUTTON_IN_WEAPON2 = 1 << 21,
    BUTTON_IN_BULLRUSH = 1 << 22,
    BUTTON_IN_GRENADE1 = 1 << 23,
    BUTTON_IN_GRENADE2 = 1 << 24,
    BUTTON_IN_ATTACK3 = 1 << 25,
};

enum e_entity_flag {
    ENTITY_FLAG_ONGROUND = 1 << 0,
    ENTITY_FLAG_DUCKING = 1 << 1,
    ENTITY_FLAG_WATERJUMP = 1 << 2,
    ENTITY_FLAG_ONTRAIN = 1 << 3,
    ENTITY_FLAG_INRAIN = 1 << 4,
    ENTITY_FLAG_FROZEN = 1 << 5,
    ENTITY_FLAG_ATCONTROLS = 1 << 6,
    ENTITY_FLAG_CLIENT = 1 << 7,
    ENTITY_FLAG_FAKECLIENT = 1 << 8,
    ENTITY_FLAG_INWATER = 1 << 9,
    ENTITY_FLAG_FLY = 1 << 10,
    ENTITY_FLAG_SWIM = 1 << 11,
    ENTITY_FLAG_CONVEYOR = 1 << 12,
    ENTITY_FLAG_NPC = 1 << 13,
    ENTITY_FLAG_GODMODE = 1 << 14,
    ENTITY_FLAG_NOTARGET = 1 << 15,
    ENTITY_FLAG_AIMTARGET = 1 << 16,
    ENTITY_FLAG_PARTIALGROUND = 1 << 17,
    ENTITY_FLAG_STATICPROP = 1 << 18,
    ENTITY_FLAG_GRAPHED = 1 << 19,
    ENTITY_FLAG_GRENADE = 1 << 20,
    ENTITY_FLAG_STEPMOVEMENT = 1 << 21,
    ENTITY_FLAG_DONTTOUCH = 1 << 22,
    ENTITY_FLAG_BASEVELOCITY = 1 << 23,
    ENTITY_FLAG_WORLDBRUSH = 1 << 24,
    ENTITY_FLAG_OBJECT = 1 << 25,
    ENTITY_FLAG_KILLME = 1 << 26,
    ENTITY_FLAG_ONFIRE = 1 << 27,
    ENTITY_FLAG_DISSOLVING = 1 << 28,
    ENTITY_FLAG_TRANSRAGDOLL = 1 << 29,
    ENTITY_FLAG_UNBLOCKABLE_BY_PLAYER = 1 << 30,
};

enum e_move_type {
    MOVE_TYPE_NONE,
    MOVE_TYPE_ISOMETRIC,
    MOVE_TYPE_WALK,
    MOVE_TYPE_STEP,
    MOVE_TYPE_FLY,
    MOVE_TYPE_FLY_GRAVITY,
    MOVE_TYPE_VPHYSICS,
    MOVE_TYPE_PUSH,
    MOVE_TYPE_NOCLIP,
    MOVE_TYPE_LADDER,
    MOVE_TYPE_OBSERVER,
    MOVE_TYPE_CUSTOM,
};

class c_user_cmd {
private:
    char padding0[0x4];

public:
    int command_number;
    int tick_count;
    vector_t view_angles;
    vector_t aim_direction;
    float forward_move;
    float side_move;
    float up_move;
    int buttons;
    unsigned char impulse;

private:
    char padding1[0x9];

public:
    int random_seed;
    short mouse_dx;
    short mouse_dy;
    bool has_been_predicted;

private:
    char padding2[0x18];
};

class c_input {
public:
    DECLARE_VFUNC(8, get_user_cmd(int slot, int sequence_number), c_user_cmd *(__thiscall *) (void *, int, int) )(slot, sequence_number);

private:
    char padding0[0xc];

public:
    bool track_ir_available;
    bool mouse_init;
    bool mouse_active;

private:
    char padding1[0x9e];

public:
    bool camera_in_third_person;
    bool camera_moving_with_mouse;
    vector_t camera_offset;
};

class c_input_system {
public:
    DECLARE_VFUNC(11, enable_input(bool enable), void(__thiscall *)(void *, bool))(enable);
};
