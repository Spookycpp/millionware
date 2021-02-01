#include "legitbot.hpp"
#include "lag compensation/lag_comp.hpp"
#include "../hitchance/hitchance.hpp"

#include "../../core/cheat.hpp"
#include "../../core/config.hpp"
#include "../../utils/math/math.hpp"
#include "../../utils/input/input.hpp"
#include "../../gui/gui.hpp"

#include <deque>

namespace features::aimbot
{

	vector3_t local_angs = vector3_t();

	using target_t = std::tuple< int, vector3_t, int >;

	float delta_time = 0.0f;
	float cur_time = 0.0f;

	bool aiming = false;
	bool flicked = false;

	std::deque< aim_record_t > angle_samples = {};

	void on_override_mouse_input(float* x, float* y)
	{
		interfaces::engine_client->get_view_angles(local_angs);

		if (local_angs.empty()) {
			return;
		}

		aimbot(x, y);
	}

	void on_create_move(user_cmd_t* cmd, c_weapon* wpn)
	{
		interfaces::engine_client->get_view_angles(local_angs);

		if (local_angs.empty()) {
			return;
		}

		flick_bot(cmd, wpn);
		standalone_rcs(cmd, wpn);
	}

	void aimbot(float* x, float* y)
	{
		aiming = false;

		if (!config::get<bool>(FNV_CT("legitbot.enabled")) || config::get<bool>(FNV_CT("ragebot.enabled"))) {
			return;
		}

		const auto wpn = (c_weapon*)cheat::local_player->active_weapon_handle().get();

		if (!wpn || !update_settings(wpn)) {
			return;
		}

		static float old_time = interfaces::global_vars->current_time;
		const float  time = interfaces::global_vars->current_time;

		delta_time = time - old_time;
		old_time = time;

		if (config::get<bool>(FNV_CT("legitbot.smoothing.enabled")))
		{
			const float avg_delta = get_average_delta() * config::get<float>(FNV_CT("legitbot.smoothing.factor"));

			if (avg_delta > delta_time) {
				delta_time = avg_delta;
			}
		}

		const auto target = get_target(config::get<int>(FNV_CT("legitbot.hitbox_method")), config::get<float>(FNV_CT("legitbot.fov")));
		const auto target_idx = std::get< 0 >(target);

		if (target_idx != -1)
		{
			if (should_activate())
			{
				aiming = true;

				if (!flicked) {
					aim_at_target(target, x, y);
				}
			}

			assist((c_player*)interfaces::entity_list->get_by_index(target_idx), x, y);
		}
		else {
			cur_time = 0.0f;
		}
	}

	void assist(c_player* target, float* x, float* y)
	{
		if (!config::get<bool>(FNV_CT("legitbot.assist.enabled"))) {
			return;
		}

		const int hitbox = get_bone(target, 1, config::get<float>(FNV_CT("legitbot.assist.fov")));

		if (hitbox == -1) {
			return;
		}

		const vector3_t local_pos = cheat::local_player->eye_angles();
		const vector3_t enemy_pos = target->get_hitbox_pos(hitbox);

		const vector3_t cur_angs = local_angs + cheat::local_player->aim_punch_angle() * 2.0f;

		vector3_t aim_angs = math::vector_angles(local_pos, enemy_pos);

		if (!math::normalize_angles(aim_angs)) {
			return;
		}

		aim_angs.x -= cheat::local_player->aim_punch_angle().x * 2.0f * (config::get<float>(FNV_CT("legitbot.rcs_x")) / 100.0f);
		aim_angs.y -= cheat::local_player->aim_punch_angle().y * 2.0f * (config::get<float>(FNV_CT("legitbot.rcs_y")) / 100.0f);

		if (!math::normalize_angles(aim_angs)) {
			return;
		}

		vector3_t view_delta = aim_angs - cur_angs;

		if (!math::normalize_angles(view_delta) || !math::clamp_angles(view_delta)) {
			return;
		}

		vector3_t move_angs = pixels_to_angles(vector2_t(*x, *y));
		move_angs *= config::get<float>(FNV_CT("legitbot.assist.strength"));

		const float delta_x = std::abs(move_angs.x);
		const float delta_y = std::abs(move_angs.y);

		vector3_t delta;
		delta.x = std::clamp(view_delta.x, -delta_x, delta_x);
		delta.y = std::clamp(view_delta.y, -delta_y, delta_y);

		const vector2_t pixels = angles_to_pixels(delta);
		*x += pixels.x;
		*y += pixels.y;
	}

	void flick_bot(user_cmd_t* cmd, c_weapon* wpn)
	{
		flicked = false;

		if (!config::get<int>(FNV_CT("legitbot.flick_bot.enabled"))) {
			return;
		}

		if (config::get<bool>(FNV_CT("legitbot.check_flashed")) && cheat::local_player->is_flashed()) {
			return;
		}

		const auto target = get_target(config::get<int>(FNV_CT("legitbot.hitbox_method")), config::get<float>(FNV_CT("legitbot.flick_bot.fov")));
		const auto target_idx = std::get< 0 >(target);
		
		if (target_idx != -1)
		{
			if (cmd->buttons & BUTTON_IN_ATTACK &&
				cheat::local_player->shots_fired() == 0 &&
				cheat::local_player->can_shoot(wpn))
			{
				flick_to_target(target, cmd, wpn, config::get<int>(FNV_CT("legitbot.flick_bot.enabled")) == 2);
				flicked = true;
			}
		}
	}

	void standalone_rcs(user_cmd_t* cmd, c_weapon* wpn)
	{
		if (wpn->is_sniper() || wpn->is_shotgun() || wpn->is_pistol()) {
			return;
		}

		if (!config::get<bool>(FNV_CT("legitbot.rcs.enabled")) || config::get<float>(FNV_CT("legitbot.rcs.x")) <= 0.0f && config::get<float>(FNV_CT("legitbot.rcs.y")) <= 0.0f) {
			return;
		}

		static vector3_t old_aim_punch;

		const int shots_fired = cheat::local_player->shots_fired();

		if (cmd->buttons & BUTTON_IN_ATTACK)
		{
			const vector3_t aim_punch = cheat::local_player->aim_punch_angle();
			const float  length = aim_punch.length_2d();

			if (length <= 0.0f || length >= 10.0f)
			{
				old_aim_punch = aim_punch;
				return;
			}

			vector3_t compensated_angs = {
				config::get<float>(FNV_CT("legitbot.rcs.x")) > 0.0f ? (aim_punch.x - old_aim_punch.x) * (2.0f * (config::get<float>(FNV_CT("legitbot.rcs.x")) / 100.0f)) : 0.0f,
				config::get<float>(FNV_CT("legitbot.rcs.y")) > 0.0f ? (aim_punch.y - old_aim_punch.y) * (2.0f * (config::get<float>(FNV_CT("legitbot.rcs.y")) / 100.0f)) : 0.0f,
				0.0f
			};

			if (!math::normalize_angles(compensated_angs))
			{
				old_aim_punch = aim_punch;
				return;
			}

			if (shots_fired > 2)
			{
				vector3_t view_angs;
				interfaces::engine_client->get_view_angles(view_angs);

				if (!math::normalize_angles(view_angs))
				{
					old_aim_punch = aim_punch;
					return;
				}

				vector3_t aim_angs = (view_angs - compensated_angs).clamp();

				if (!math::normalize_angles(aim_angs) || !math::clamp_angles(aim_angs))
				{
					old_aim_punch = aim_punch;
					return;
				}

				interfaces::engine_client->set_view_angles(aim_angs);
			}

			old_aim_punch = aim_punch;
		}
		else {
			old_aim_punch = { 0.0f, 0.0f, 0.0f };
		}
	}

	bool update_settings(c_weapon* wpn)
	{
		return true;
		// @Note: Need to figure this one out.
	}

	void aim_at_target(const target_t& data, float* x, float* y)
	{
		const auto ent = (c_player*)interfaces::entity_list->get_by_index(std::get<0>(data));

		if (!is_valid_target(ent)) {
			return;
		}

		vector3_t aim_angs = math::vector_angles(
			cheat::local_player->eye_angles(),
			ent->extrapolate_position(std::get< 1 >(data))
		);

		if (!math::normalize_angles(aim_angs)) {
			return;
		}

		aim_angs.x -= cheat::local_player->aim_punch_angle().x * 2.0f * (config::get<float>(FNV_CT("legitbot.rcs_x")) / 100.0f);
		aim_angs.y -= cheat::local_player->aim_punch_angle().y * 2.0f * (config::get<float>(FNV_CT("legitbot.rcs_y")) / 100.0f);

		if (!math::normalize_angles(aim_angs)) {
			return;
		}

		vector3_t delta = aim_angs - local_angs;

		if (!math::normalize_angles(delta) || !math::clamp_angles(delta)) {
			return;
		}

		const float delta_length = delta.length();

		if (delta_length > 0.0f)
		{
			cur_time += delta_time;

			if (config::get<float>(FNV_CT("legitbot.speed")) != 0)
			{
				const float final_time = delta_length / (config::get<float>(FNV_CT("legitbot.speed")) / 75.0f);

				if (cur_time > final_time) {
					cur_time = final_time;
				}

				float aim_progress = cur_time / final_time;
				aim_progress = math::ease_in(0.0f, 1.0f, aim_progress, config::get<float>(FNV_CT("legitbot.speed_exponent")));

				delta *= aim_progress;
			}

			const vector2_t pixels = angles_to_pixels(delta);
			*x += pixels.x;
			*y += pixels.y;
		}
	}

	void flick_to_target(const target_t& data, user_cmd_t* cmd, c_weapon* wpn, bool silent)
	{
		const auto ent = (c_player*)interfaces::entity_list->get_by_index(std::get<0>(data));

		if (!is_valid_target(ent)) {
			return;
		}

		vector3_t aim_angs = math::vector_angles(
			cheat::local_player->eye_angles(),
			ent->extrapolate_position(std::get< 1 >(data))
		);

		if (!math::normalize_angles(aim_angs)) {
			return;
		}

		aim_angs.x -= cheat::local_player->aim_punch_angle().x * 2.0f * (config::get<float>(FNV_CT("legitbot.rcs_x")) / 100.0f);
		aim_angs.y -= cheat::local_player->aim_punch_angle().y * 2.0f * (config::get<float>(FNV_CT("legitbot.rcs_y")) / 100.0f);

		if (!math::normalize_angles(aim_angs) || !math::clamp_angles(aim_angs)) {
			return;
		}

		if (config::get<int>(FNV_CT("legitbot.flick_bot.hit_chance")) > 0)
		{
			if (!features::hit_chance::can_hit(ent, wpn, aim_angs, config::get<int>(FNV_CT("legitbot.flick_bot.hit_chance")), std::get< 2 >(data))) {
				return;
			}
		}

		cmd->view_angles = aim_angs;

		if (!silent) {
			interfaces::engine_client->set_view_angles(cmd->view_angles);
		}
	}

	target_t get_target(int method, float fov)
	{
		target_t best_target{ -1, vector3_t(), -1 };
		float    best_fov = fov;

		const vector3_t local_pos = cheat::local_player->eye_angles();
		const vector3_t cur_angs = local_angs + cheat::local_player->aim_punch_angle() * 2.0f;

		for (int i = 1; i < 65; ++i)
		{
			const auto ent = (c_player*)interfaces::entity_list->get_by_index(i);

			if (!is_valid_target(ent)) {
				continue;
			}

			const int hitbox = get_bone(ent, method, fov);

			if (hitbox == -1) {
				continue;
			}

			vector3_t aim_pos;

			if (!config::get<bool>(FNV_CT("legitbot.target_backtrack"))) {
				aim_pos = ent->get_hitbox_pos(hitbox);
			}
			else {
				aim_pos = lag_comp::get_backtracked_position(i);
			}

			if (aim_pos.empty())
			{
				aim_pos = ent->get_hitbox_pos(hitbox);

				if (aim_pos.empty()) {
					continue;
				}
			}

			//if (!config::get<bool>(FNV_CT("legitbot->check_visible")))
			//{
			//	if (!ent->is_visible(cheat::local_player, cheat::local_player->eye_angles(), aim_pos)) {
			//		continue;
			//	}
			//}

			if (!config::get<bool>(FNV_CT("legitbot.check_smoked"))) {
				if (util::line_goes_through_smoke(cheat::local_player->eye_angles(), aim_pos)) {
					continue;
				}
			}

			vector3_t aim_angs = math::vector_angles(local_pos, aim_pos);

			if (!math::normalize_angles(aim_angs)) {
				continue;
			}

			const float cur_fov = math::get_fov(cur_angs, aim_angs);

			if (!std::isfinite(cur_fov))
			{
				best_target = std::make_tuple(i, aim_pos, hitbox);
				break;
			}

			if (cur_fov < best_fov)
			{
				best_target = std::make_tuple(i, aim_pos, hitbox);
				best_fov = cur_fov;
			}
		}

		return best_target;
	}

	int get_bone(c_player* target, int method, float fov)
	{
		int   best_hitbox = -1;
		float best_fov = fov;

		if (method == 0)
		{
			switch (config::get<int>(FNV_CT("legitbot.hitbox")))
			{
			case 0: best_hitbox = HEAD; break;
			case 1: best_hitbox = NECK; break;
			case 2: best_hitbox = U_CHEST; break;
			case 3: best_hitbox = L_CHEST; break;
			case 4: best_hitbox = STOMACH; break;
			default: return -1;
			}
		}
		else if (method == 1)
		{
			static std::vector< int > hitboxes = {
				HEAD, NECK, PELVIS, L_CHEST, U_CHEST
			};

			const vector3_t local_pos = cheat::local_player->eye_angles();
			const vector3_t cur_angs  = local_angs + cheat::local_player->aim_punch_angle() * 2.0f;

			for (auto& idx : hitboxes)
			{
				vector3_t aim_pos = target->get_hitbox_pos(idx);

				if (aim_pos.empty()) {
					continue;
				}

				vector3_t aim_angs = math::vector_angles(local_pos, aim_pos);

				if (!math::normalize_angles(aim_angs)) {
					continue;
				}

				const float cur_fov = math::get_fov(cur_angs, aim_angs);

				if (!std::isfinite(cur_fov))
				{
					best_hitbox = idx;
					break;
				}

				if (cur_fov < best_fov)
				{
					best_hitbox = idx;
					best_fov = cur_fov;
				}
			}
		}

		return best_hitbox;
	}

	bool should_activate(bool check_hotkey_only)
	{
		if (gui::is_visible()) {
			return false;
		}

		if (config::get<int>(FNV_CT("legitbot.hotkey_hold")) > 0 && config::get<int>(FNV_CT("legitbot.hotkey_toggle")) > 0)
		{
			static bool toggle = false;

			if (GetAsyncKeyState(config::get<int>(FNV_CT("legitbot.hotkey_toggle"))) & 0x1 /*input::get_key_info( legitbot->hotkey_toggle ).is_pressed( )*/) {
				toggle = !toggle;
				// Beep( toggle ? 750 : 500, 300 );
			}

			if (!input::is_key_down(config::get<int>(FNV_CT("legitbot.hotkey_hold"))))
			{
				if (!toggle) {
					return false;
				}
			}
		}
		else
		{
			if (config::get<int>(FNV_CT("legitbot.hotkey_hold")) > 0)
			{
				if (!input::is_key_down(config::get<int>(FNV_CT("legitbot.hotkey_hold")))) {
					return false;
				}
			}
			else if (config::get<int>(FNV_CT("legitbot.hotkey_toggle")) > 0)
			{
				static bool toggle = false;

				if (GetAsyncKeyState(config::get<int>(FNV_CT("legitbot.hotkey_toggle"))) & 0x1 /*input::get_key_info( legitbot->hotkey_toggle ).is_pressed( )*/) {
					toggle = !toggle;
					// Beep( toggle ? 750 : 500, 300 );
				}

				if (!toggle) {
					return false;
				}
			}
		}

		if (!check_hotkey_only)
		{
			if (config::get<bool>(FNV_CT("legitbot.check_flashed")) && cheat::local_player->is_flashed()) {
				return false;
			}

			if (cheat::local_player->shots_fired() < config::get<int>(FNV_CT("legitbot.start_bullets"))) {
				return false;
			}
		}

		return true;
	}

	bool is_valid_target(c_player* target) 
	{
		if (!target || target == cheat::local_player) {
			return false;
		}

		if (!target->is_valid()) {
			return false;
		}

		const static auto mp_teammates_are_enemies = interfaces::convar_system->find(STR_ENC("mp_teammates_are_enemies"));

		if (config::get<bool>(FNV_CT("legitbot.check_team")))
		{
			if (mp_teammates_are_enemies->get_int() == 0 && target->team_num() == cheat::local_player->team_num()) {
				return false;
			}
		}

		return true;
	}

	void sample_angle_data(const vector3_t& angles)
	{
		const float time = interfaces::global_vars->current_time;

		if (!aiming) {
			angle_samples.push_front({ angles, time });
		}

		while (static_cast<int>(angle_samples.size()) > config::get<int>(FNV_CT("legitbot.smoothing.samples"))) {
			angle_samples.pop_back();
		}
	}

	float get_average_delta()
	{
		if (angle_samples.empty()) {
			return 0.0f;
		}

		float avg_delta = 0.0f;

		for (auto& it : angle_samples)
		{
			static vector3_t last_angs = it.view_angles;

			const float time_delta = interfaces::global_vars->current_time - it.time;
			const float delta_diff = delta_time / (time_delta * 2.0f);

			vector3_t   delta = (last_angs - it.view_angles).clamp();
			const float	delta_length = delta.length() * delta_diff;

			avg_delta += delta_length;
			last_angs = it.view_angles;
		}

		return avg_delta / float(config::get<int>(FNV_CT("legitbot.smoothing.samples")));
	}

	vector2_t angles_to_pixels(const vector3_t& angles)
	{
		static auto m_yaw	= interfaces::convar_system->find(STR_ENC("m_yaw"));
		static auto m_pitch = interfaces::convar_system->find(STR_ENC("m_pitch"));

		const float x = angles.x / m_pitch->get_float();
		const float y = angles.y / m_yaw->get_float();

		return vector2_t(-y, x);
	}

	vector3_t pixels_to_angles(const vector2_t& pixels)
	{
		static auto m_yaw   = interfaces::convar_system->find(STR_ENC("m_yaw"));
		static auto m_pitch = interfaces::convar_system->find(STR_ENC("m_pitch"));

		const float x = pixels.x * m_pitch->get_float();
		const float y = pixels.y * m_yaw->get_float();

		return vector3_t(-y, x, 0.0f).clamp();
	}

}