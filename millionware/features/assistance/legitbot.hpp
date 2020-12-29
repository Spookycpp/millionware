#pragma once
#include <tuple>

#include "../../sdk/vector.hpp"
#include "../../sdk/entity.hpp"
#include "../../sdk/user_cmd.hpp"

namespace features::aimbot
{
	struct aim_record_t {
		vector3_t view_angles;
		float	  time;
	};

	using target_t = std::tuple< int, vector3_t, int >;

	void     on_override_mouse_input(float* x, float* y);
	void     on_create_move(user_cmd_t* cmd, c_weapon* wpn);

	void     aimbot(float* x, float* y);
	void     assist(c_player* target, float* x, float* y);
	void     flick_bot(user_cmd_t* cmd, c_weapon* wpn);
	void     standalone_rcs(user_cmd_t* cmd, c_weapon* wpn);

	bool     update_settings(c_weapon* wpn);

	void     aim_at_target(const target_t& data, float* x, float* y);
	void     flick_to_target(const target_t& data, user_cmd_t* cmd, c_weapon* wpn, bool silent);

	target_t get_target(int method, float fov);
	int      get_bone(c_player* target, int method, float fov);

	bool     should_activate(bool check_hotkey_only = false);
	bool     is_valid_target(c_player* target);

	void     sample_angle_data(const vector3_t& angles);
	float    get_average_delta();

	vector2_t   angles_to_pixels(const vector3_t& angles);
	vector3_t   pixels_to_angles(const vector2_t& pixels);
}