#pragma once

#include "../../source engine/color.h"

struct settings_t {
	struct {
		color_t accent_color = { 255, 0, 245, 255 };
		bool weapon_groups	 = false;
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
			bool  draw_visible = false;
			bool  draw_teammates = false;
			bool  damage_logs = false;
			bool  engine_radar = false;
			bool  bounding_box = false;
			bool  player_name = false;
			bool  health = false;
			bool  armor = false;
			bool  weapon = false;
			bool  ammo = false;
			bool  skeleton = false;
			bool  head_spot = false;
			bool  barrel = false;
			bool  icon_flags = false;
			bool  money = false;
			bool  kevlar = false;
			bool  helmet = false;
			bool  flashed = false;
			bool  smoked = false;
			bool  ping = false;
			bool  c4_carrier = false;
			bool  footsteps = false;
			bool  visualize_backtrack = false;
			bool  glow = false;
			bool  outside_fov = false;
			float outside_fov_radius = 0.75f;
			int   outside_fov_size = 25;

			struct {
				int   material = 0;
				float reflectivity = 0.0f;
				float luminance = 0.0f;
				bool  render_in_smoke = false;
				bool  visualize_backtrack = false;
				bool  visualize_all_records = false;
			} chams;
		} player;

		struct {
			int   skybox = 0;
			bool  night_mode = false;
			float night_mode_intensity = 0.075f;
			int   weapon = 0;
			bool  grenade = false;
			bool  bomb = false;
			bool  defusal_kit = false;
			bool  chicken = false;
			bool  remove_fog = false;
		} world;

		struct {
			bool recoil_crosshair = false;
			bool penetration_crosshair = false;
			bool sniper_crosshair = false;
			bool grenade_prediction = false;
			bool spectator_list = false;
			bool kill_effect = false;
			bool disable_post_processing = false;
			bool disable_dynamic_shadows = false;
			bool disable_panorama_blur   = false;
			bool disable_fog			 = false;
			int  flash_alpha  = 100;
			float aspect_ratio = 0.f;
			float override_fov = 90.0f;
			int feet_fx = 0;
		} local;
	} visuals;

	struct {
		bool  auto_pistol = false;
		bool  rank_reveal = false;
		bool  auto_accept = false;
		bool  unlock_inventory = false;
		bool  ragdoll_push = false;
		bool  ragdoll_float = false;
		bool  buy_log = false;
		bool  bomb_log = false;
		bool  bomb_log_sounds = false;
		bool  report_player_on_death = false;
		int   hit_sound = 0;
		int   kill_sound = 0;

		struct {
			bool  enabled = false;
			int   hotkey = 0;
			float distance = 80.0f;
		} third_person;

		struct {
			bool  bunny_hop = false;
			bool  jump_bug = false;
			int   jump_bug_hotkey = 0;
			bool  edge_bug = false;
			int	  edge_bug_hotkey = 0;
			bool  edge_jump = false;
			int	  edge_jump_hotkey = 0;

			bool  edge_bug_assist = false;
			bool  edge_bug_crouch = false;
			bool  edge_bug_mouse = false;
			bool  edge_bug_movement = false;
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

	struct {
		struct {
			float team[4] = { 0.16f, 0.49f, 0.8f, 1.0f };
			float team_visible[4] = { 0.16f, 0.8f, 0.61f, 1.0f };
			float enemy[4] = { 0.8f, 0.78f, 0.16f, 1.0f };
			float enemy_visible[4] = { 0.8f, 0.16f, 0.16f, 1.0f };

			float chams_team[4] = { 0.16f, 0.49f, 0.8f, 1.0f };
			float chams_team_visible[4] = { 0.16f, 0.8f, 0.61f, 1.0f };
			float chams_enemy[4] = { 0.8f, 0.78f, 0.16f, 1.0f };
			float chams_enemy_visible[4] = { 0.8f, 0.16f, 0.16f, 1.0f };

			float glow_team[4] = { 0.44f, 0.6f, 0.86f, 0.6f };
			float glow_team_visible[4] = { 0.44f, 0.6f, 0.86f, 0.6f };
			float glow_enemy[4] = { 0.87f, 0.69f, 0.34f, 0.6f };
			float glow_enemy_visible[4] = { 0.87f, 0.69f, 0.34f, 0.6f };
		} player;
	} color;
};

extern settings_t              settings;
extern settings_t::legitbot_t* settings_lbot;