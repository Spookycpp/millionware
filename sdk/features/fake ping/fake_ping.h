#pragma once

#include "../../source engine/client_dll.h"

namespace features::fake_ping {
    struct incoming_sequence_t {
        float current_time;
        int in_reliable_state;
        int in_sequence_num;
    };

    void on_create_move();
    void on_send_datagram(c_net_channel *net_channel);

    void update_sequences();
    void update_net_channel(c_net_channel *net_channel);
} // namespace features::fake_ping