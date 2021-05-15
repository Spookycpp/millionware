#pragma once

#include <string>

#include "../../source engine/color.h"
#include "../../features/inventory changer/item_definitions.h"
#include "../../features/inventory changer/kit_parser.h"

struct settings_t {
	struct {
		color_t accent_color = { 255, 0, 245, 255 };
		bool weapon_groups	 = false;
		int  sound_fx_volume = 100;
	} global;

	struct legitbot_t {
		bool  enabled = false;
		int   hotkey  = 1;
		bool  check_visible = false;
		bool  check_team = false;
		bool  check_flashed = false;
		bool  check_smoked = false;
		int   hitbox_method = 0;
		int   hitbox = 0;
		bool  target_backtrack = false;
		int   start_bullets = 0;
		float fov = 10.0f;
		float speed = 10.0f;
		float speed_exponent = 1.0f;
		float rcs_x = 100.0f;
		float rcs_y = 100.0f;

		struct {
			int   enabled = 0;
			float fov = 1.5f;
			int   hit_chance = 30;
		} flick_bot;

		struct {
			bool  enabled = false;
			float fov = 10.0f;
			float strength = 0.3f;
		} assist;

		struct {
			bool  enabled = false;
			float fov = 10.0f;
			int   time = 200;
		} backtrack;

		struct {
			bool  enabled = false;
			int   samples = 12;
			float factor = 1.0f;
		} smoothing;

		struct {
			bool  enabled = false;
			float x = 30.0f;
			float y = 30.0f;
		} rcs;

		struct {
			bool  enabled = false;
			int   hotkey = 18;
			bool  check_team = false;
			bool  check_flashed = false;
			bool  check_smoked = false;
			int   hit_chance = 30;
			int   delay = 0;

			struct {
				bool enabled = false;
				int  time = 200;
			} backtrack;
		} triggerbot;

	} lbot_global, lbot_pistols, lbot_hpistols, lbot_rifles, lbot_smg, lbot_shotgun, lbot_awp, lbot_scout, lbot_auto, lbot_other;

	struct {
		bool  enabled = false;
		int   hotkey = 0;
		bool  check_visible = false;
		bool  check_team = false;
		bool  auto_aim = false;
		bool  auto_shoot = false;
		bool  silent_aim = false;
		bool  no_recoil = false;
		int   target_method = 1;
		int   hitbox_method = 0;
		int   hitbox = 0;
		float fov = 180.0f;
	} ragebot;

	struct {
		bool stream_proof = false;

		struct {
			int   activation_type = 0;
			int   hotkey = 0;
			bool  engine_radar = false;

			bool  bounding_box = false;
            color_t bounding_box_color = {255, 255, 255};

			bool  player_name = false;
            color_t player_name_color = {255, 255, 255};

			bool  health = false;

			bool  weapon = false;

			bool  ammo = false;
            color_t ammo_color = {87, 230, 225};

			bool  armor = false;
            int	  flags = 0;

			bool  skeleton = false;
            color_t skeleton_color = {255, 255, 255};

			bool  head_spot = false;
            color_t head_spot_color = {255, 255, 255};

			bool  barrel = false;

			bool  glow = false;
            bool  damage_logs = false;
			bool  outside_fov = false;
			float outside_fov_radius = 0.75f;
			int   outside_fov_size = 25;

			struct {
				int  material  = 0;
				bool visible   = false;
				color_t visible_color = { 255, 102, 0, 255 };
				bool invisible = false;
				color_t invisible_color = { 222, 7, 208, 255 };
				bool smoke     = false;
				bool backtrack = false;
			} chams;
		} player;

		struct {
			int     skybox = 0;
			bool    nightmode  = false;
			float	nightmode_darkness = 0.f;
			color_t nightmode_color = { 38, 34, 34 };
			bool    fullbright = false;
			bool    fog = false;
			int     fog_length = 0;
			color_t fog_color = { 255, 255, 255 };
			int     weapon = 0;
			bool    grenade = false;
			bool    bomb = false;
			bool    defusal_kit = false;
			bool    chicken = false;
			bool    remove_fog = false;
		} world;

		struct {
			bool recoil_crosshair = false;
			bool penetration_crosshair = false;
			bool sniper_crosshair = false;
			bool grenade_prediction = false;
			bool spectator_list = false;
			bool kill_effect = false;
			bool viewmodel_offset = false;
			bool disable_post_processing = false;
			bool disable_panorama_blur   = false;
			int indicators = 0;
            bool velocity_takeoff		 = false;
			bool jb_indicator			 = false;
			bool eb_indicator			 = false;
			bool ej_indicator			 = false;
			bool mj_indicator			 = false;
			int  flash_alpha  = 100;
			float aspect_ratio = 0.f;
			int   override_fov = 90;
			float viewmodel_offset_x = 0.f;
			float viewmodel_offset_y = 0.f;
			float viewmodel_offset_z = 0.f;
			float viewmodel_offset_r = 0.f; 
			int feet_fx = 0;

			struct {
                bool enabled = false;
                bool first_person_on_nade = false;
                int hotkey = 0;
                float distance = 80.0f;
            } third_person;

		} local;
	} visuals;

	struct {
		bool  player_privacy = false;
		bool  auto_pistol = false;
		bool  rank_reveal = false;
		bool  money_reveal = false;
		bool  preserve_killfeed = false;
		bool  auto_accept = false;
		bool  unlock_hidden_convars = false;
		bool  unlock_inventory = false;
		bool  ragdoll_push = false;
		bool  ragdoll_float = false;
		bool  buy_log = false;
		bool  bomb_log = false;
		bool  bomb_log_sounds = false;
		bool  report_player_on_death = false;
		bool  vote_reveal = false;
		int   hit_sound = 0;
        int kill_sound = 0;
		std::string hit_sound_custom;
		std::string kill_sound_custom;

		struct {
			bool  bunny_hop = false;
			bool  jump_bug = false;
			int   jump_bug_hotkey = 0;
			bool  edge_bug = false;
			int	  edge_bug_hotkey = 0;
			bool  edge_jump = false;
			int	  edge_jump_hotkey = 0;

			bool  edge_bug_assist = false;
			int   edge_bug_assist_hotkey = 0;
			int   edge_bug_radius = 0;
			float edgebug_rage_amount = 0.f;

			int   auto_strafe = 0;
			bool  air_duck = false;
			bool  no_duck_cooldown = false;
			bool  fast_walk = false;
			int   fast_walk_hotkey = 16;
			bool  slide_walk = false;
		} movement;

		struct {
			int   type = 0;
			int   ticks = 0;
			int   fluctuate = 0;
			bool  in_air = false;
			bool  on_peek = false;
			int   on_peek_ticks = 0;
			bool  on_peek_alert = false;
			float on_peek_minimum_speed = 115.0f;
			bool  on_move = false;
			bool  on_attack = false;
			bool  avoid_ground = false;
			bool  jump_reset = false;
			bool  on_duck = false;
			bool  choke_indicator = false;
		} fake_lag;

		struct {
			bool enabled = false;
		} fake_ping;

		struct {
			bool enabled = false;
			bool balance = false;
			int  swap_sides_hotkey = 0x58; // x
			int  range = 0;
			bool desync_indicator = false;
		} legit_aa;

		struct {
			bool  enabled = false;
			int   hotkey_hold = 5;
			int   hotkey_toggle = 0;
			bool  on_peek = false;
			int   tick_shift = 12;
			int   wait_ticks = 3;
		} rapid_lag;
	} miscellaneous;
};

struct sticker_t {
    template <sync_type_t sync_type> void update() {
        do_sync<sync_type>(kit_parser::sticker_kits, kit_vector_index, kit, &kit_parser::paint_kit_t::id);
    }

    int kit = 0;
    int kit_vector_index = 0;
    float wear = 0.0f;
    float scale = 1.0f;
    float rotation = 0.0f;
};

struct inventory_item_t {
    template <sync_type_t sync_type> void update() {
        const std::vector<kit_parser::paint_kit_t> *kit_names;
        const std::vector<item_definitions::item_name_t> *item_names;

        switch (type) {
        case 0:
            kit_names = &kit_parser::skin_kits;
            item_names = &item_definitions::weapon_names;
            break;
        case 1:
            kit_names = &kit_parser::skin_kits;
            item_names = &item_definitions::knife_names;
            break;
        case 2:
            kit_names = &kit_parser::glove_kits;
            item_names = &item_definitions::glove_names;
            break;
        default:
            return;
        }

        do_sync<sync_type>(*item_names, definition_vector_idx, definition, &item_definitions::item_name_t::id);

        do_sync<sync_type>(*kit_names, paint_kit_vector_idx, paint_kit, &kit_parser::paint_kit_t::id);

        do_sync<sync_type>(item_definitions::quality_names, quality_vector_idx, quality, &item_definitions::quality_name_t::id);

        do_sync<sync_type>(item_definitions::rarity_names, rarity_vector_idx, rarity, &item_definitions::rarity_name_t::id);

        if (type == 0) {
            for (auto &sticker : stickers) {
                sticker.update<sync_type>();
            }
        }
    }

    void update_title() {
        if (paint_kit_vector_idx > 0) {
            title.clear();

            switch (type) {
            case 0:
                title += item_definitions::weapon_names.at(definition_vector_idx).name;
                title += " | ";
                title += kit_parser::skin_kits.at(paint_kit_vector_idx).name;
                break;
            case 1:
                title += item_definitions::knife_names.at(definition_vector_idx).name;
                title += " | ";
                title += kit_parser::skin_kits.at(paint_kit_vector_idx).name;
                break;
            case 2:
                title += item_definitions::glove_names.at(definition_vector_idx).name;
                title += " | ";
                title += kit_parser::glove_kits.at(paint_kit_vector_idx).name;
                break;
            default:
                break;
            }

            if (strlen(custom_name) != 0) {
                title += " (" + std::string(custom_name) + ")";
            }
        }
        else {
            title = XORSTR("Unassigned");
        }
    }

    int auto_equip = 0;
    std::string title = XORSTR("Unassigned");
    int type = 0;
    int definition = 0;
    int definition_vector_idx = 0;
    int paint_kit = 0;
    int paint_kit_vector_idx = 0;
    int quality = 0;
    int quality_vector_idx = 0;
    int rarity = 0;
    int rarity_vector_idx = 0;
    int seed = 0;
    int stat_trak = 0;
    float wear = 0.0f;
    char custom_name[32] = "";
    std::array<sticker_t, 5> stickers;
};

extern settings_t              settings;
extern settings_t::legitbot_t* settings_lbot;