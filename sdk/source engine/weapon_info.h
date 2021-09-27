#pragma once

struct weapon_info_t {
public:
private:
    char pad_0000[4];

public:
    char *weapon_name;

private:
    char pad_0008[12];

public:
    int max_clip_ammo;

private:
    char pad_0018[12];

public:
    int max_ammo;

private:
    char pad_0028[4];

public:
    char *world_model;
    char *view_model;
    char *dropped_model;

private:
    char pad_0038[4];
    char *N00000010;
    char pad_0040[56];

public:
    char *empty_sound;

private:
    char pad_007C[4];

public:
    char *ammo_type;

private:
    char pad_0084[4];

public:
    char *hud_name;
    char *weapon_name2;

private:
    char pad_0090[60];

public:
    int type;
    int price;
    int kill_reward;
    char *anim_prefix;
    float cycle_time;
    float cycle_time_alt;
    float time_to_idle;
    float idle_interval;
    bool full_auto;

private:
    char pad_00E9[3];

public:
    int damage;
    float headshot_multiplier;
    float armor_ratio;

private:
    char pad_00F4[4];

public:
    float penetration;

private:
    char pad_00FC[8];

public:
    float range;
    float range_modifier;
    float throw_velocity;

private:
    char pad_010C[12];

public:
    bool has_silencer;

private:
    char pad_011D[15];

public:
    float max_speed;
    float max_speed_alt;
    float spread;
    float spread_alt;
    float inaccuracy_crouch;
    float inaccuracy_crouch_alt;
    float inaccuracy_stand;
    float inaccuracy_stand_alt;
    float inaccuracy_jump_start;
    float inaccuracy_jump;
    float inaccuracy_jump_alt;
    float inaccuracy_land;
    float inaccuracy_land_alt;
    float inaccuracy_ladder;
    float inaccuracy_ladder_alt;
    float inaccuracy_fire;
    float inaccuracy_fire_alt;
    float inaccuracy_move;
    float inaccuracy_move_alt;
    float inaccuracy_reload;
    int recoil_seed;

private:
    char pad_0180[60];

public:
    int zoom_levels;
    int zoom_fov1;
    int zoom_fov2;
};