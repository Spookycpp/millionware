#include "lag_comp.hpp"
#include "../legitbot.hpp"

#include "../../../core/config.hpp"

#include <tuple>

namespace features::aimbot::lag_comp
{
	std::array< std::deque< lag_record_t >, 65 > records = {};

	void on_create_move(user_cmd_t* cmd)
	{
		if (settings_legitbot->backtrack.enabled || settings_legitbot->triggerbot.backtrack.enabled) {
			store_records();
		}

		if (!settings_legitbot->backtrack.enabled) {
			return;
		}

		const auto target = get_target(1, settings_legitbot->backtrack.fov);
		const auto target_idx = std::get< 0 >(target);

		if (target_idx != -1)
		{
			if (cmd->buttons & IN_ATTACK) {
				backtrack_entity(cmd, target_idx);
			}
		}
	}

	lag_record_t* find_best_record(const int ent_idx)
	{
		if (records[ent_idx].empty()) {
			return nullptr;
		}

		lag_record_t* best_record = nullptr;
		float         best_fov = settings_legitbot->backtrack.fov;

		const vec3_t local_pos = ctx::client.local->get_eye_pos();
		const vec3_t local_angs = c_game::instance().get_engine()->get_view_angles();

		const int cur_tick_count = c_game::instance().get_global_vars()->tick_count;

		for (auto& it : records[ent_idx])
		{
			const auto net_channel = c_game::instance().get_engine()->get_net_channel_info();

			if (!net_channel) {
				continue;
			}

			float max_latency;

			if (settings.misc.fake_ping.enabled) {
				max_latency = 0.4f + net_channel->get_latency(FLOW_OUTGOING);
			}
			else {
				max_latency = static_cast<float>(settings_legitbot->backtrack.time) * 0.001f + net_channel->get_latency(FLOW_OUTGOING);
			}

			if (std::abs(cur_tick_count - it.tick_count) > TIME_TO_TICKS(max_latency)) {
				continue;
			}

			if (!it.is_valid()) {
				continue;
			}

			vec3_t aim_angs = math::vector_angles(local_pos, it.position);

			if (!math::normalize_angles(aim_angs)) {
				continue;
			}

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
		if (!can_backtrack_entity(ent_idx)) {
			return false;
		}

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
		if (records[ent_idx].empty()) {
			return false;
		}

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

		if (!best_record) {
			return vec3_t();
		}

		return best_record->aimbot_position;

		/*if ( records [ ent_idx ].empty( ) ) {
			return vec3_t( );
		}

		vec3_t best_pos = vec3_t( );
		float  best_fov = settings_legitbot->backtrack.fov;

		const vec3_t local_pos  = ctx::client.local->get_eye_pos( );
		const vec3_t local_angs = c_game::instance( ).get_engine( )->get_view_angles( );

		const vec3_t cur_angs   = local_angs + ctx::client.local->get_aim_punch_ang( ) * 2.0f;

		for ( auto &it : util::misc::reverse_iterator( records [ ent_idx ] ) )
		{
			if ( !it.is_valid( ) || it.aimbot_position.empty( ) ) {
				continue;
			}

			vec3_t aim_angs = math::vector_angles( local_pos, it.aimbot_position );

			if ( !math::normalize_angles( aim_angs ) ) {
				continue;
			}

			const float cur_fov = math::get_fov( cur_angs, aim_angs );

			if ( cur_fov > 0.0f && cur_fov < best_fov )
			{
				best_pos = it.aimbot_position;
				best_fov = cur_fov;
			}
		}

		return best_pos;*/
	}

	void store_records()
	{
		for (int i = 1; i < 65; ++i)
		{
			auto ent = c_game::instance().get_entity_list()->get< C_CSPlayer >(i);

			if (ent && ent != ctx::client.local && ent->is_valid())
			{
				const int    idx = ent->get_index();
				const vec3_t pos = ent->get_hitbox_pos(head);
				const vec3_t aimbot_pos = ent->get_hitbox_pos(get_bone(ent, settings_legitbot->hitbox_method, settings_legitbot->backtrack.fov));

				lag_record_t record = {};

				if (ent->setup_bones(record.matrices.data(), record.matrices.size(), 0x100, 0.0f))
				{
					record.position = pos;
					record.aimbot_position = aimbot_pos;
					record.tick_count = TIME_TO_TICKS(ent->get_simulation_time());
					record.mins = ent->get_mins();
					record.maxs = ent->get_maxs();

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