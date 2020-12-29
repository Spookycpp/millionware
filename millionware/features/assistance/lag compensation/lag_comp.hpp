#pragma once

#include "../../../core/interfaces.hpp"
#include "../../../core/cheat.hpp"

#include "../../../sdk/user_cmd.hpp"
#include "../../../sdk/vector.hpp"
#include "../../../sdk/matrix.hpp"

#include <deque>
#include <array>
#include <algorithm>

#define FLOW_OUTGOING	0
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out

float TICK_INTERVAL() {
	return interfaces::global_vars->interval_per_tick;
}

int TIME_TO_TICKS(const float dt) {
	return static_cast<int>(0.5f + dt / TICK_INTERVAL());
}

float TICKS_TO_TIME(const int tick) {
	return tick * TICK_INTERVAL();
}

float get_total_latency()
{
	c_net_channel_info* nci = interfaces::engine_client->get_net_channel_info();

	if (!nci) {
		return 0.0f;
	}

	return nci->get_latency(FLOW_OUTGOING) + nci->get_latency(FLOW_INCOMING);
}

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

		[[nodiscard]] bool is_valid() const {
			const float correct = std::clamp(get_total_latency(), 0.0f, 0.2f); // todo: account for non valveds
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