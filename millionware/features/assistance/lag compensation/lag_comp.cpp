#include <tuple>

#include "lag_comp.hpp"
#include "../legitbot.hpp"

#include "../../../core/config.hpp"
#include "../../../utils/math/math.hpp"
#include "../../../sdk/entity.hpp"

namespace features::aimbot::lag_comp
{
	std::array< std::deque< lag_record_t >, 65 > records = {};

	void on_create_move(user_cmd_t* cmd)
	{
		if (config::get<bool>(FNV_CT("legitbot.backtrack.enabled")) || config::get<bool>(FNV_CT("legitbot.triggerbot.backtrack.enabled")))
			store_records();

		if (!config::get<bool>(FNV_CT("legitbot.backtrack.enabled")))
			return;

		const auto target = get_target(1, config::get<float>(FNV_CT("legitbot.backtrack.fov")));
		const auto target_idx = std::get< 0 >(target);
		
		if (target_idx != -1)
		{
			if (cmd->buttons & BUTTON_IN_ATTACK) {
				backtrack_entity(cmd, target_idx);
			}
		}
	}

	lag_record_t* find_best_record(const int ent_idx)
	{
		if (records[ent_idx].empty())
			return nullptr;

		lag_record_t* best_record = nullptr;
		float         best_fov = config::get<float>(FNV_CT("legitbot.backtrack.fov"));

		const vector3_t local_pos = cheat::local_player->get_eye_origin();
		vector3_t local_angs; interfaces::engine_client->get_view_angles(local_angs);

		const int cur_tick_count = interfaces::global_vars->tick_count;

		for (auto& it : records[ent_idx])
		{
			const auto net_channel = interfaces::engine_client->get_net_channel_info();

			if (!net_channel)
				continue;

			float max_latency;

			if (config::get<bool>(FNV_CT("settings.misc.fake_ping.enabled"))) {
				max_latency = 0.4f + net_channel->get_latency(FLOW_OUTGOING);
			}
			else {
				max_latency = static_cast<float>(config::get<int>(FNV_CT("legitbot.backtrack.time"))) * 0.001f + net_channel->get_latency(FLOW_OUTGOING);
			}

			if (std::abs(cur_tick_count - it.tick_count) > TIME_TO_TICKS(max_latency))
				continue;

			if (!it.is_valid()) {
				continue;
			}

			vector3_t aim_angs = math::vector_angles(local_pos, it.position);

			if (!math::normalize_angles(aim_angs))
				continue;

			const float cur_fov = math::get_fov(local_angs, aim_angs);

			if (!std::isfinite(cur_fov))
			{
				best_record = &it;
				break;
			}

			if (cur_fov < best_fov)
			{
				best_record = &it;
				best_fov = cur_fov;
			}
		}

		return best_record;
	}

	bool backtrack_entity(user_cmd_t* cmd, const int ent_idx)
	{
		if (!can_backtrack_entity(ent_idx))
			return false;

		const auto best_record = find_best_record(ent_idx);

		if (best_record)
		{
			cmd->tick_count = best_record->tick_count;
			return true;
		}

		return false;
	}

	bool can_backtrack_entity(const int ent_idx)
	{
		if (records[ent_idx].empty())
			return false;

		for (auto& it : records[ent_idx])
		{
			if (it.is_valid()) {
				return true;
			}
		}

		return false;
	}

	vector3_t get_backtracked_position(const int ent_idx)
	{
		const auto best_record = find_best_record(ent_idx);

		if (!best_record)
			return vector3_t();

		return best_record->aimbot_position;

	}

	void store_records()
	{
		for (int i = 1; i < 65; ++i)
		{
			const auto ent = (c_player*)interfaces::entity_list->get_by_index(i);
	
			if (ent && ent != cheat::local_player && ent->is_valid())
			{
				const auto idx = ent->networkable()->index();
				const auto pos = ent->get_hitbox_pos(HEAD);
				const auto aimbot_pos = ent->get_hitbox_pos(get_bone(ent, config::get<int>(FNV_CT("legitbot.hitbox_method")), config::get<float>(FNV_CT("legitbot.backtrack.fov"))));
	
				lag_record_t record = {};
	
				if (ent->renderable()->setup_bones(record.matrices.data(), record.matrices.size(), 0x100, 0.0f))
				{
					const auto collidable = ent->get_collideable();

					record.position = pos;
					record.aimbot_position = aimbot_pos;
					record.tick_count = TIME_TO_TICKS(ent->get_simulation_time());
					record.mins = collidable->get_mins();
					record.maxs = collidable->get_maxs();
	
					records[idx].push_front(record);
				}
	
				while (!records[idx].empty() && static_cast<int>(records[idx].size()) > TIME_TO_TICKS(1.0f)) {
					records[idx].pop_back();
				}
			}
			else {
				records[i].clear();
			}
		}
	}

	std::deque< lag_record_t >& get_record(const int ent_idx) {
		return records[ent_idx];
	}
}