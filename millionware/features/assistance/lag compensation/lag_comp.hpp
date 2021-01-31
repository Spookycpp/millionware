#pragma once

#include "../../../core/interfaces.hpp"
#include "../../../core/cheat.hpp"

#include "../../../utils/util.hpp"

#include "../../../sdk/user_cmd.hpp"
#include "../../../sdk/vector.hpp"
#include "../../../sdk/matrix.hpp"

#include <deque>
#include <array>
#include <algorithm>

namespace features::aimbot::lag_comp
{
	struct lag_record_t
	{
		int	tick_count = 0;
		vector3_t position = vector3_t();
		vector3_t aimbot_position = vector3_t();
		std::array< matrix3x4_t, 128 > matrices = {};
		vector3_t mins = vector3_t();
		vector3_t maxs = vector3_t();

		bool is_valid() const {
			const float correct = std::clamp(util::get_total_latency(), 0.0f, 0.2f); // todo: account for non valveds
			const float delta	= correct - (float(cheat::local_player->tick_base()) * interfaces::global_vars->interval_per_tick - TICKS_TO_TIME(tick_count));

			return std::abs(delta) < 0.2f;
		}
	};

	void on_create_move(user_cmd_t* cmd);

	lag_record_t* find_best_record(int ent_idx);
	bool backtrack_entity(user_cmd_t* cmd, int ent_idx);
	bool can_backtrack_entity(int ent_idx);
	vector3_t get_backtracked_position(int ent_idx);

	void store_records();

	std::deque< lag_record_t >& get_record(int ent_idx);
}