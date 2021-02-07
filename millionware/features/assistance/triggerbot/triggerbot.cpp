#include "triggerbot.hpp"

#include "../../lag compensation/lag_compensation.hpp"
#include "../lag compensation/lag_comp.hpp"

#include "../../hitchance/hitchance.hpp"

#include "../../../core/config.hpp"
#include "../../../utils/util.hpp"
#include "../../../utils/input/input.hpp"
#include "../../../sdk/engine_trace.hpp"

#include "../../../gui/gui.hpp"

namespace features::aimbot::triggerbot
{
	int64_t current_time = 0;

	void on_create_move(user_cmd_t* cmd, c_weapon* wpn)
	{
		if (!config::get<bool>(FNV_CT("legitbot.triggerbot.enabled"))) {
			return;
		}

		if (!wpn->is_valid() && wpn->item_definition_index() != WEAPON_TASER) {
			return;
		}

		// update timer
		{
			const auto now = std::chrono::system_clock::now();
			const auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

			current_time = now_ms.time_since_epoch().count();
		}

		if (should_activate()) {
			think(cmd, wpn);
		}
	}

	void think(user_cmd_t* cmd, c_weapon* wpn)
	{
		const auto send_attack = [&]() -> void {
			cmd->buttons |= wpn->item_definition_index() != WEAPON_REVOLVER ? BUTTON_IN_ATTACK : BUTTON_IN_ATTACK2;
		};

		const auto unsend_attack = [&]() -> void {
			cmd->buttons &= wpn->item_definition_index() != WEAPON_REVOLVER ? ~BUTTON_IN_ATTACK : ~BUTTON_IN_ATTACK2;
		};

		const auto wpn_info = wpn ? interfaces::weapon_system->get_weapon_info(wpn->item_definition_index()) : nullptr;;

		if (!wpn_info) {
			return;
		}

		vector3_t view_angs;
		interfaces::engine_client->get_view_angles(view_angs);

		const vector3_t fwd = math::angle_to_vector(view_angs + cheat::local_player->aim_punch_angle() * 2.0f) * wpn_info->range;

		const vector3_t src = cheat::local_player->get_eye_origin();
		const vector3_t dst = src + fwd;

		if (!config::get<bool>(FNV_CT("legitbot.triggerbot.check_smoked")))
		{
			if (util::line_goes_through_smoke(src, dst)) {
				return;
			}
		}

		if (!cheat::local_player->can_shoot(wpn))
		{
			unsend_attack();
			return;
		}

		const bool should_shoot = !config::get<bool>(FNV_CT("legitbot.triggerbot.backtrack.enabled")) ?
			trace_to_target(wpn, src, dst) :
			trace_to_backtracked_target(cmd, wpn, src, dst);

		if (should_shoot)
		{
			if (config::get<int>(FNV_CT("legitbot.triggerbot.delay")) > 0)
			{
				static int64_t delayed_time = 0;

				if (current_time > delayed_time)
				{
					delayed_time = current_time + config::get<int>(FNV_CT("legitbot.triggerbot.delay"));
					send_attack();
				}
			}
			else {
				send_attack();
			}
		}
	}

	bool trace_to_target(c_weapon* wpn, const vector3_t& start_pos, const vector3_t& end_pos)
	{
		c_trace_filter filter;
		filter.skip = cheat::local_player;

		trace_t tr;
		interfaces::engine_trace->trace_ray(ray_t{ start_pos, end_pos }, MASK_SHOT, &filter, &tr);

		if (!is_valid_target(tr.hit_ent)) {
			return false;
		}

		if (config::get<int>(FNV_CT("legitbot.triggerbot.hit_chance")) > 0)
		{
			vector3_t aim_angs = math::vector_angles(start_pos, end_pos);

			if (!math::normalize_angles(aim_angs)) {
				return false;
			}

			if (!hit_chance::can_hit(static_cast<c_player*>(tr.hit_ent), wpn, aim_angs, config::get<int>(FNV_CT("legitbot.triggerbot.hit_chance")), tr.m_hitbox)) {
				return false;
			}
		}

		return tr.hit_group <= HIT_GROUP_RIGHTLEG && tr.hit_group > HIT_GROUP_GENERIC;
	}

	bool trace_to_backtracked_target(user_cmd_t* cmd, c_weapon* wpn, const vector3_t& start_pos, const vector3_t& end_pos)
	{
		float max_latency;

		if (config::get<int>(FNV_CT("misc.fake_ping.enabled"))) {
			max_latency = 0.300f + interfaces::engine_client->get_net_channel_info()->get_latency(FLOW_OUTGOING);
		}
		else {
			max_latency = static_cast<float>(config::get<bool>(FNV_CT("legitbot.triggerbot.backtrack.time"))) * 0.001f + interfaces::engine_client->get_net_channel_info()->get_latency(FLOW_OUTGOING);
		}

		vector3_t aim_angs = math::vector_angles(start_pos, end_pos);

		if (!math::normalize_angles(aim_angs)) {
			return false;
		}

		const int cur_tick_count = interfaces::global_vars->tick_count;

		for (int i = 1; i < 65; ++i)
		{
			const auto ent = (c_player*)interfaces::entity_list->get_by_index(i);

			if (!is_valid_target(ent)) {
				continue;
			}

			for (auto& it : features::aimbot::lag_comp::get_record(i))
			{
				if (!it.is_valid()) {
					continue;
				}

				if (std::abs(cur_tick_count - it.tick_count) > TIME_TO_TICKS(max_latency)) {
					continue;
				}

				const auto model = ent->renderable()->get_model();
				if (!model) {
					continue;
				}

				const auto hdr = interfaces::model_info->get_studio_model(model);

				if (!hdr) {
					continue;
				}

				const auto set = hdr->hitbox_set(ent->hitbox_set());

				if (!set) {
					continue;
				}

				for (int j = 0; j < set->hitbox_count; ++j)
				{
					const auto box = set->hitbox(j);

					if (!box || box->radius == -1.0f) {
						continue;
					}

					const vector3_t min = math::vector_transform(box->mins, it.matrices[box->bone]);
					const vector3_t max = math::vector_transform(box->maxs, it.matrices[box->bone]);

					const float radius = box->radius;

					if (auto intersection = util::get_intersection(start_pos, end_pos, min, max, radius); intersection)
					{
						c_trace_filter filter;
						filter.skip = cheat::local_player;

						trace_t tr;
						interfaces::engine_trace->trace_ray(ray_t{ start_pos, *intersection }, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

						if (config::get<int>(FNV_CT("legitbot.triggerbot.hit_chance")) > 0)
						{
							auto hit_ent = static_cast<c_player*>(tr.hit_ent);

							if (hit_ent->is_valid() && !hit_chance::can_hit(hit_ent, wpn, aim_angs, config::get<int>(FNV_CT("legitbot.triggerbot.hit_chance")), tr.m_hitbox)) {
								continue;
							}
						}

						if (tr.fraction <= 0.97f) {
							continue;
						}

						features::aimbot::lag_comp::backtrack_entity(cmd, i);

						return true;
					}
				}
			}
		}

		return false;
	}

	bool is_valid_target(c_entity* target)
	{
		if (!target || target == cheat::local_player) {
			return false;
		}

		if (!static_cast<c_player*>(target)->is_player()) {
			return false;
		}

		auto player = static_cast<c_player*>(target);

		if (!player->is_valid() || player->has_gun_game_immunity()) {
			return false;
		}

		const static auto mp_teammates_are_enemies = interfaces::convar_system->find(STR_ENC("mp_teammates_are_enemies"));

		if (config::get<bool>(FNV_CT("legitbot.triggerbot.check_team")))
		{
			if (mp_teammates_are_enemies->get_int() == 0 && target->team_num() == cheat::local_player->team_num()) {
				return false;
			}
		}

		return true;
	}

	bool should_activate()
	{
		if (gui::is_visible()) {
			return false;
		}

		if (!input::is_hotkey_active(FNV_CT("triggerbot.hotkey")))
		{
			return false;
		}

		if (config::get<int>(FNV_CT("legitbot.triggerbot.check_flashed")))
		{
			if (cheat::local_player->is_flashed()) {
				return false;
			}
		}

		return true;
	}
}