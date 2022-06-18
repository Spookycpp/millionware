#include "fake_ping.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"

#include "../../core/util/util.h"

#include <vector>

namespace features::fake_ping {
    std::vector<incoming_sequence_t> sequences = {};

    int last_sequence = 0;

    void on_create_move() {
        update_sequences();
    }

    void on_send_datagram(c_net_channel *net_channel) {
        update_net_channel(net_channel);
    }

    void update_sequences() {
        const auto net_channel = interfaces::client_state->net_channel;

        if (net_channel->in_sequence_num != last_sequence) {
            last_sequence = net_channel->in_sequence_num;
            sequences.push_back({interfaces::global_vars->current_time, net_channel->in_reliable_state, net_channel->in_sequence_num});
        }

        for (size_t i = 0; i < sequences.size(); ++i) {
            const float delta = interfaces::global_vars->current_time - sequences.at(i).current_time;
            const float max_ping = settings.miscellaneous.fake_ping.max_ping / 1000;

            if (delta > max_ping) {
                sequences.erase(sequences.begin() + i);
            }
        }
    }

    void update_net_channel(c_net_channel *net_channel) {
        if (!settings.miscellaneous.fake_ping.enabled)
            return;

        if (cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
            return;

        const float cur_time = interfaces::global_vars->current_time;
        const float desired_ping = settings.miscellaneous.fake_ping.max_ping / 1000;
        const float latency = desired_ping - (interfaces::engine_client->get_net_channel_info()->get_latency(FLOW_OUTGOING) + interfaces::global_vars->frame_time) - TICK_INTERVAL();

        for (auto &it : sequences) {
            const float delta = cur_time - it.current_time;

            if (delta >= latency) {
                net_channel->in_reliable_state = it.in_reliable_state;
                net_channel->in_sequence_num = it.in_sequence_num;
                break;
            }
        }
    }
} // namespace features::fake_ping