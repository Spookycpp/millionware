#pragma once

#include <string>

#include "../../engine/security/xorstr.h"
#include "../../features/inventory changer/item_definitions.h"
#include "../../features/inventory changer/kit_parser.h"
#include "../../source engine/color.h"

struct settings_t {
    struct {
        std::string config_name = xs("default");
        color_t accent_color = {222, 102, 122, 255};
        bool weapon_groups = false;
        bool debug_overlay = false;
    } global;

    struct legitbot_t {
        bool enabled = false;
        int hotkey = 1;
        bool check_visible = false;
        bool check_team = false;
        bool check_flashed = false;
        bool check_smoked = false;
        int hitbox_method = 0;
        int hitbox = 0;
        bool target_backtrack = false;
        int start_bullets = 0;
        float fov = 10.0f;
        float speed = 10.0f;
        float speed_exponent = 1.0f;
        float rcs_x = 100.0f;
        float rcs_y = 100.0f;

        struct {
            int enabled = 0;
            float fov = 1.5f;
            int hit_chance = 30;
        } flick_bot;

        struct {
            bool enabled = false;
            float fov = 5.0f;
            float strength = 0.3f;
        } assist;

        struct {
            bool enabled = false;
            float fov = 10.0f;
            int time = 200;
        } backtrack;

        struct {
            bool enabled = false;
            int samples = 12;
            float factor = 1.0f;
        } smoothing;

        struct {
            bool enabled = false;
            float x = 30.0f;
            float y = 30.0f;
        } rcs;

        struct {
            bool enabled = false;
            int hotkey = 18;
            bool check_team = false;
            bool check_flashed = false;
            bool check_smoked = false;
            int hit_chance = 30;
            int delay = 0;

            struct {
                bool enabled = false;
                int time = 200;
            } backtrack;
        } triggerbot;

    } lbot_global, lbot_pistols, lbot_hpistols, lbot_rifles, lbot_smg, lbot_shotgun, lbot_awp, lbot_scout, lbot_auto, lbot_other;

    struct {
        bool enabled = false;
        int hotkey = 0;
        bool check_visible = false;
        bool check_team = false;
        bool auto_aim = false;
        bool auto_shoot = false;
        bool silent_aim = false;
        bool no_recoil = false;
        int target_method = 1;
        int hitbox_method = 0;
        int hitbox = 0;
        float fov = 180.0f;
    } ragebot;

    struct {
        bool stream_proof = false;

        struct {
            int activation_type = 0;
            bool draw_teammates = false;
            int hotkey = 0;
            bool engine_radar = false;

            bool bounding_box = false;
            color_t bounding_box_color = {238, 116, 211, 200};

            bool player_name = false;
            color_t player_name_color = {230, 230, 230};

            bool health = false;

            bool weapon = false;

            bool ammo = false;
            color_t ammo_color = {64, 143, 255};

            bool armor = false;
            int flags = 0;

            bool skeleton = false;
            color_t skeleton_color = {255, 255, 255};

            bool head_spot = false;
            color_t head_spot_color = {255, 255, 255};

            bool glow = false;
            color_t glow_color = {222, 102, 122, 150};

            bool damage_logs = false;
            bool outside_fov = false;
            float outside_fov_radius = 0.75f;
            int outside_fov_size = 25;

            bool footsteps = false;
            color_t footsteps_color = {221, 12, 57, 150};

            struct {
                int material = 0;
                bool visible = false;
                color_t visible_color = {255, 102, 0, 255};
                bool invisible = false;
                color_t invisible_color = {222, 7, 208, 255};
                bool smoke = false;
                bool backtrack = false;
                bool glow = false;
                color_t glow_color = {222, 102, 122, 255};
            } chams;
        } player;

        struct {
            int skybox = 0;
            bool nightmode = false;
            float nightmode_darkness = 80.f;
            bool fullbright = false;
            bool fog = false;
            int fog_length = 100;
            color_t fog_color = {255, 255, 255};
            bool weapon = false;
            color_t weapon_color = {255, 255, 255};
            bool grenades = false;
            color_t grenades_color = {119, 227, 40};
            bool molotov_spread = false;
            color_t molotov_spread_color = {255, 0, 0, 75};
            bool planted_bomb = false;
            color_t planted_bomb_color = {255, 255, 255};
            bool dropped_bomb = false;
            color_t dropped_bomb_color = {255, 255, 255};
            bool defusal_kit = false;
            color_t defusal_kit_color = {255, 255, 255};
            bool chicken = false;
            bool remove_fog = false;
            int  smoke_type = 0;
            bool weather = false;
        } world;

        struct {
            int recoil_crosshair = 0;
            bool penetration_crosshair = false;
            bool sniper_crosshair = false;
            bool grenade_prediction = false;
            color_t grenade_prediction_color = {222, 102, 122};
            bool spectator_list = false;
            bool kill_effect = false;
            bool viewmodel_offset = false;
            bool disable_post_processing = false;
            bool disable_panorama_blur = false;
            int indicators = 0;
            bool velocity_takeoff = false;
            color_t velocity_color_1 = {255, 199, 89};
            color_t velocity_color_2 = {255, 119, 119};
            color_t velocity_color_3 = {30, 255, 109};
            bool jb_indicator = false;
            bool eb_indicator = false;
            bool ej_indicator = false;
            bool mj_indicator = false;
            int flash_alpha = 100;
            float aspect_ratio = 0.f;
            int override_fov = 90;
            float viewmodel_offset_x = 0.f;
            float viewmodel_offset_y = 0.f;
            float viewmodel_offset_z = 0.f;
            float viewmodel_offset_r = 0.f;
            int feet_fx = 0;
            bool data_graph = false;
            bool data_graph_info = false;
            bool data_graph_background = false;
            color_t data_graph_color = {255, 255, 255};
            color_t data_graph_bg_color = {27, 27, 27, 210};

            struct {
                bool enabled = false;
                bool first_person_on_nade = false;
                int hotkey = 0;
                float distance = 80.0f;
            } third_person;

            struct {
                bool arms = false;
                int  arms_material = 0;
                color_t arms_color = {56, 159, 255};

                bool weapon = false;
                int weapon_material = 0;
                color_t weapon_color = {28, 99, 255};

                bool sleeve = false;
                int sleeve_material = 0;
                color_t sleeve_color = {150, 130, 255};
            } chams;

        } local;
    } visuals;

    struct {
        bool player_privacy = false;
        bool auto_pistol = false;
        bool clantag = false;
        bool rank_reveal = false;
        bool money_reveal = false;
        bool preserve_killfeed = false;
        bool auto_accept = false;
        bool reveal_overwatch_suspect = false;
        bool unlock_hidden_convars = false;
        bool unlock_inventory = false;
        bool ragdoll_push = false;
        bool ragdoll_float = false;
        bool buy_log = false;
        bool bomb_log = false;
        bool bomb_log_sounds = false;
        bool bomb_holder_log = false;
        bool report_player_on_death = false;
        bool vote_reveal = false;
        bool discord_rpc = false;
        bool kill_say = false;
        bool custom_netgraph = false;
        int hit_sound = 0;
        int kill_sound = 0;
        int server_regions = 0;
        std::string hit_sound_custom;
        std::string kill_sound_custom;

        struct {
            bool bunny_hop = false;
            bool jump_bug = false;
            int jump_bug_hotkey = 0;
            bool edge_bug = false;
            int edge_bug_hotkey = 0;
            bool edge_jump = false;
            int edge_jump_hotkey = 0;
            bool edge_jump_duck = false;
            int edge_jump_duck_hotkey = 0;
            bool long_jump = false;
            int long_jump_hotkey = 0;
            bool edge_jump_lj_bind = false;
            int edge_jump_lj_bind_hotkey = 0;

            bool edge_bug_assist = false;
            int edge_bug_assist_hotkey = 0;
            int edge_bug_radius = 0;
            float edgebug_rage_amount = 0.f;

            bool auto_strafe = false;
            bool air_duck = false;
            bool no_duck_cooldown = false;
            bool slide_walk = false;

            bool strafe_optimizer = false;
            int strafe_optimizer_key = 0;
            float strafe_optimizer_max_gain = 0.f;
            float strafe_optimizer_pull_amount = 0.f;
            int strafe_optimizer_max_velocity = 0;

            bool blockbot = false;
            int blockbot_key = 0;
        } movement;

        struct {
            int type = 0;
            int ticks = 0;
            int fluctuate = 0;
            bool in_air = false;
            bool on_peek = false;
            int on_peek_ticks = 0;
            bool on_peek_alert = false;
            float on_peek_minimum_speed = 115.0f;
            bool on_move = false;
            bool on_attack = false;
            bool avoid_ground = false;
            bool jump_reset = false;
            bool on_duck = false;
            bool choke_indicator = false;
        } fake_lag;

        struct {
            bool enabled = false;
            float max_ping = 100.f;
        } fake_ping;

        struct {
            bool enabled = false;
            bool balance = false;
            int swap_sides_hotkey = 0x58; // x
            int range = 0;
            bool desync_indicator = false;
        } legit_aa;

        struct {
            bool enabled = false;
            int hotkey_hold = 5;
            int hotkey_toggle = 0;
            bool on_peek = false;
            int tick_shift = 12;
            int wait_ticks = 3;
        } rapid_lag;

    } miscellaneous;

    void save();
    void load();
};

extern settings_t settings;
extern settings_t::legitbot_t *settings_lbot;