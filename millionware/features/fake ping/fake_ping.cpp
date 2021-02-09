#include "fake_ping.hpp"

#include "../../core/cheat.hpp"
#include "../../core/config.hpp"
#include "../../core/interfaces.hpp"

#include "../../utils/util.hpp"

namespace features::fake_ping
{
	std::vector< incoming_sequence_t > sequences = {};

	int last_sequence = 0;

	void on_create_move() {
		update_sequences();
	}

	void on_send_datagram(c_net_channel* net_channel) {
		update_net_channel(net_channel);
	}

	void update_sequences()
	{
		const auto net_channel = interfaces::client_state->net_channel;

		if (net_channel->in_sequence_num != last_sequence)
		{
			last_sequence = net_channel->in_sequence_num;
			sequences.push_back({ interfaces::global_vars->current_time, net_channel->in_reliable_state, net_channel->in_sequence_num });
		}

		for (size_t i = 0; i < sequences.size(); ++i)
		{
			const float delta = interfaces::global_vars->current_time - sequences.at(i).current_time;

			if (delta > 0.2f) {
				sequences.erase(sequences.begin() + i);
			}
		}
	}

	void update_net_channel(c_net_channel* net_channel)
	{
		if (!config::get<bool>(FNV_CT("misc.other.fake_ping"))) {
			return;
		}

		if (!cheat::local_player->is_alive()) {
			return;
		}

		const float cur_time = interfaces::global_vars->current_time;
		const float latency = 0.15f - (interfaces::engine_client->get_net_channel_info()->get_latency(FLOW_OUTGOING) + interfaces::global_vars->frame_time) - TICK_INTERVAL();

		for (auto& it : sequences)
		{
			const float delta = cur_time - it.current_time;

			if (delta >= latency)
			{
				net_channel->in_reliable_state = it.in_reliable_state;
				net_channel->in_sequence_num = it.in_sequence_num;
				break;
			}
		}
	}
}