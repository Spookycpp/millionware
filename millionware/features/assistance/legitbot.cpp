#include "legitbot.hpp"
#include "lag compensation/lag_comp.hpp"
#include "hitchance/hitchance.hpp"

#include "../../core/config.hpp"
#include "../../core/cheat.hpp"

#include <tuple>

namespace features::aimbot
{
	vector3_t local_angs = vector3_t();

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

		if (!config::get<bool>(FNV_CT("aimbot.legit.enabled")) || config::get<bool>(FNV_CT("aimbot.legit.enabled"))) {
			return;
		}

		const auto wpn = cheat::local_player->get_active_weapon();

		if (!wpn || !update_settings(wpn)) {
			return;
		}

		static float old_time = interfaces::global_vars->current_time;
		const float  time	  = interfaces::global_vars->current_time;

		delta_time = time - old_time;
		old_time   = time;

		if (settings_legitbot->smoothing.enabled)
		{
			const float avg_delta = get_average_delta() * settings_legitbot->smoothing.factor;

			if (avg_delta > delta_time) {
				delta_time = avg_delta;
			}
		}

		const auto target = get_target(settings_legitbot->hitbox_method, settings_legitbot->fov);
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

			assist(interfaces::entity_list->get< c_player >(target_idx), x, y);
		}
		else {
			cur_time = 0.0f;
		}
	}

	void assist(c_player* target, float* x, float* y)
	{
		if (!settings_legitbot->assist.enabled) {
			return;
		}

		const int hitbox = get_bone(target, 1, settings_legitbot->assist.fov);

		if (hitbox == -1) {
			return;
		}

		const vector3_t local_pos = ctx::client.local->get_eye_pos();
		const vector3_t enemy_pos = target->get_hitbox_pos(hitbox);

		const vector3_t cur_angs = local_angs + ctx::client.local->get_aim_punch_ang() * 2.0f;

		vector3_t aim_angs = math::vector_angles(local_pos, enemy_pos);

		if (!math::normalize_angles(aim_angs)) {
			return;
		}

		aim_angs.x -= ctx::client.local->get_aim_punch_ang().x * 2.0f * (settings_legitbot->rcs_x / 100.0f);
		aim_angs.y -= ctx::client.local->get_aim_punch_ang().y * 2.0f * (settings_legitbot->rcs_y / 100.0f);

		if (!math::normalize_angles(aim_angs)) {
			return;
		}

		vector3_t view_delta = aim_angs - cur_angs;

		if (!math::normalize_angles(view_delta) || !math::clamp_angles(view_delta)) {
			return;
		}

		vector3_t move_angs = pixels_to_angles(vec2_t(*x, *y));
		move_angs *= settings_legitbot->assist.strength;

		const float delta_x = std::abs(move_angs.x);
		const float delta_y = std::abs(move_angs.y);

		vector3_t delta;
		delta.x = std::clamp(view_delta.x, -delta_x, delta_x);
		delta.y = std::clamp(view_delta.y, -delta_y, delta_y);

		const vector2_t pixels = angles_to_pixels(delta);
		*x += pixels.x;
		*y += pixels.y;
	}

	void flick_bot(user_cmd_t* cmd, c_player* wpn)
	{
		flicked = false;

		if (!settings_legitbot->flick_bot.enabled) {
			return;
		}

		if (settings_legitbot->check_flashed && ctx::client.local->is_flashed()) {
			return;
		}

		const auto target = get_target(settings_legitbot->hitbox_method, settings_legitbot->flick_bot.fov);
		const auto target_idx = std::get< 0 >(target);

		if (target_idx != -1)
		{
			if (cmd->buttons & BUTTON_IN_ATTACK &&
				ctx::client.local->get_shots_fired() == 0 &&
				ctx::client.local->can_shoot(wpn))
			{
				flick_to_target(target, cmd, wpn, settings_legitbot->flick_bot.enabled == 2);
				flicked = true;
			}
		}
	}

	void standalone_rcs(user_cmd_t* cmd, c_weapon* wpn)
	{
		if (wpn->is_sniper() || wpn->is_shotgun() || wpn->is_pistol()) {
			return;
		}

		if (!settings_legitbot->rcs.enabled || settings_legitbot->rcs.x <= 0.0f && settings_legitbot->rcs.y <= 0.0f) {
			return;
		}

		static vector3_t old_aim_punch;

		const int shots_fired = ctx::client.local->get_shots_fired();

		if (cmd->buttons & BUTTON_IN_ATTACK)
		{
			const vector3_t aim_punch = ctx::client.local->get_aim_punch_ang();
			const float  length = aim_punch.length_2d();

			if (length <= 0.0f || length >= 10.0f)
			{
				old_aim_punch = aim_punch;
				return;
			}

			vector3_t compensated_angs = {
				settings_legitbot->rcs.x > 0.0f ? (aim_punch.x - old_aim_punch.x) * (2.0f * (settings_legitbot->rcs.x / 100.0f)) : 0.0f,
				settings_legitbot->rcs.y > 0.0f ? (aim_punch.y - old_aim_punch.y) * (2.0f * (settings_legitbot->rcs.y / 100.0f)) : 0.0f,
				0.0f
			};

			if (!math::normalize_angles(compensated_angs))
			{
				old_aim_punch = aim_punch;
				return;
			}

			if (shots_fired > 2)
			{
				vector3_t view_angs = interfaces::engine_client->get_view_angles();
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

				c_game::instance().get_engine()->set_view_angles(aim_angs);
			}

			old_aim_punch = aim_punch;
		}
		else {
			old_aim_punch.init(0.0f, 0.0f, 0.0f);
		}
	}

	bool update_settings(c_weapon* wpn)
	{
		if (wpn->is_grenade() || wpn->is_knife() || wpn->get_clip() <= 0) {
			return false;
		}

		if (settings.global.weapon_groups)
		{
			if (wpn->is_rifle()) {
				settings_legitbot = &settings.legitbot_rifles;
			}
			else if (wpn->is_sniper()) {
				settings_legitbot = &settings.legitbot_snipers;
			}
			else if (wpn->is_pistol()) {
				settings_legitbot = &settings.legitbot_pistols;
			}
			else {
				settings_legitbot = &settings.legitbot_other;
			}
		}
		else {
			settings_legitbot = &settings.legitbot_global;
		}

		return settings_legitbot != nullptr;
	}

	void aim_at_target(const target_t& data, float* x, float* y)
	{
		const auto ent = c_game::instance().get_entity_list()->get< C_CSPlayer >(std::get< 0 >(data));

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

		aim_angs.x -= cheat::local_player->aim_punch_angle().x * 2.0f * (settings_legitbot->rcs_x / 100.0f);
		aim_angs.y -= cheat::local_player->aim_punch_angle().y * 2.0f * (settings_legitbot->rcs_y / 100.0f);

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

			if (settings_legitbot->speed != 0)
			{
				const float final_time = delta_length / (settings_legitbot->speed / 75.0f);

				if (cur_time > final_time) {
					cur_time = final_time;
				}

				float aim_progress = cur_time / final_time;
				aim_progress = math::ease_in(0.0f, 1.0f, aim_progress, settings_legitbot->speed_exponent);

				delta *= aim_progress;
			}

			const vec2_t pixels = angles_to_pixels(delta);
			*x += pixels.x;
			*y += pixels.y;
		}
	}

	void flick_to_target(const target_t& data, user_cmd_t* cmd, c_weapon* wpn, const bool silent)
	{
		const auto ent = c_game::instance().get_entity_list()->get< C_CSPlayer >(std::get< 0 >(data));

		if (!is_valid_target(ent)) {
			return;
		}

		vector3_t aim_angs = math::vector_angles(
			ctx::client.local->get_eye_pos(),
			ent->extrapolate_position(std::get< 1 >(data))
		);

		if (!math::normalize_angles(aim_angs)) {
			return;
		}

		aim_angs.x -= cheat::local_player->aim_punch_angle().x * 2.0f * (settings_legitbot->rcs_x / 100.0f);
		aim_angs.y -= cheat::local_player->aim_punch_angle().y * 2.0f * (settings_legitbot->rcs_y / 100.0f);

		if (!math::normalize_angles(aim_angs) || !math::clamp_angles(aim_angs)) {
			return;
		}

		if (settings_legitbot->flick_bot.hit_chance > 0)
		{
			if (!hit_chance::can_hit(ent, wpn, aim_angs, settings_legitbot->flick_bot.hit_chance, std::get< 2 >(data))) {
				return;
			}
		}

		cmd->view_angs = aim_angs;

		if (!silent) {
			c_game::instance().get_engine()->set_view_angles(cmd->view_angs);
		}
	}

	target_t get_target(const int method, const float fov)
	{
		target_t best_target{ -1, vector3_t(), -1 };
		float    best_fov = fov;

		const vector3_t local_pos = cheat::local_player->eye_angles();
		const vector3_t cur_angs = local_angs + cheat::local_player->aim_punch_angle() * 2.0f;

		for (int i = 1; i < 65; ++i)
		{
			const auto ent = interfaces::entity_list->get< c_player >(i);

			if (!is_valid_target(ent)) {
				continue;
			}

			const int hitbox = get_bone(ent, method, fov);

			if (hitbox == -1) {
				continue;
			}

			vector3_t aim_pos;

			if (!settings_legitbot->target_backtrack) {
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

			if (!settings_legitbot->check_visible)
			{
				if (!ent->is_visible(cheat::local_player, cheat::local_player->eye_angles(), aim_pos)) {
					continue;
				}
			}

			if (!settings_legitbot->check_smoked)
			{
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

	int get_bone(c_player* target, const int method, const float fov)
	{
		int   best_hitbox = -1;
		float best_fov = fov;

		if (method == 0)
		{
			switch (settings_legitbot->hitbox)
			{
			case 0: best_hitbox = head; break;
			case 1: best_hitbox = neck; break;
			case 2: best_hitbox = u_chest; break;
			case 3: best_hitbox = l_chest; break;
			case 4: best_hitbox = stomach; break;
			default: return -1;
			}
		}
		else if (method == 1)
		{
			static std::vector< int > hitboxes = {
				head, neck, pelvis, l_chest, u_chest
			};

			const vector3_t local_pos = cheat::local_player->eye_angles();
			const vector3_t cur_angs = local_angs + ctx::client.local->get_aim_punch_ang() * 2.0f;

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

	bool should_activate(const bool check_hotkey_only)
	{
		if (c_menu::instance().is_active()) {
			return false;
		}

		if (settings_legitbot->hotkey_hold > 0 && settings_legitbot->hotkey_toggle > 0)
		{
			static bool toggle = false;

			if (GetAsyncKeyState(settings_legitbot->hotkey_toggle) & 0x1 /*input::get_key_info( settings_legitbot->hotkey_toggle ).is_pressed( )*/) {
				toggle = !toggle;
				// Beep( toggle ? 750 : 500, 300 );
			}

			if (!input::get_key_info(settings_legitbot->hotkey_hold).is_held())
			{
				if (!toggle) {
					return false;
				}
			}
		}
		else
		{
			if (settings_legitbot->hotkey_hold > 0)
			{
				if (!input::get_key_info(settings_legitbot->hotkey_hold).is_held()) {
					return false;
				}
			}
			else if (settings_legitbot->hotkey_toggle > 0)
			{
				static bool toggle = false;

				if (GetAsyncKeyState(settings_legitbot->hotkey_toggle) & 0x1 /*input::get_key_info( settings_legitbot->hotkey_toggle ).is_pressed( )*/) {
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
			if (settings_legitbot->check_flashed && ctx::client.local->is_flashed()) {
				return false;
			}

			if (ctx::client.local->get_shots_fired() < settings_legitbot->start_bullets) {
				return false;
			}
		}

		return true;
	}

	bool is_valid_target(c_player* target)
	{
		if (!target || target == ctx::client.local) {
			return false;
		}

		if (!target->is_valid()) {
			return false;
		}

		if (!settings_legitbot->check_team)
		{
			if (!c_game::instance().teammates_are_enemies() && target->get_team() == ctx::client.local->get_team()) {
				return false;
			}
		}

		return true;
	}

	void sample_angle_data(const vector3_t& angles)
	{
		const float time = c_game::instance().get_global_vars()->current_time;

		if (!aiming) {
			angle_samples.push_front({ angles, time });
		}

		while (static_cast<int>(angle_samples.size()) > settings_legitbot->smoothing.samples) {
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

			const float time_delta = c_game::instance().get_global_vars()->current_time - it.time;
			const float delta_diff = delta_time / (time_delta * 2.0f);

			vector3_t	    delta = (last_angs - it.view_angles).clamp();
			const float	delta_length = delta.length() * delta_diff;

			avg_delta += delta_length;
			last_angs = it.view_angles;
		}

		return avg_delta / float(settings_legitbot->smoothing.samples);
	}

	vector2_t angles_to_pixels(const vector3_t& angles)
	{
		static auto m_yaw = interfaces::convar_system->find(XOR("m_yaw"));
		static auto m_pitch = interfaces::convar_system->find(XOR("m_pitch"));

		const float x = angles.x / m_pitch->get_float();
		const float y = angles.y / m_yaw->get_float();

		return vector2_t(-y, x);
	}

	vector3_t pixels_to_angles(const vector2_t& pixels)
	{
		static auto m_yaw = interfaces::convar_system->find(XOR("m_yaw"));
		static auto m_pitch = interfaces::convar_system->find(XOR("m_pitch"));

		const float x = pixels.x * m_pitch->get_float();
		const float y = pixels.y * m_yaw->get_float();

		return vector3_t(-y, x, 0.0f).clamp();
	}
}