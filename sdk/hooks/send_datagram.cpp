#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"

#include "../engine/debug/debug_overlay.h"

#include "../core/util/util.h"

#include "../features/fake ping/fake_ping.h"

int __fastcall hooks::send_datagram(c_net_channel *ecx, uintptr_t *edx, void *buffer) {

    PROFILE_WITH(send_datagram);

    if (buffer || !interfaces::engine_client->is_in_game())
        return send_datagram_original(ecx, edx, buffer);

    if (interfaces::client_state->delta_tick <= 0)
        return send_datagram_original(ecx, edx, buffer);

    const int backup_sequence = ecx->in_sequence_num;
    const int backup_reliable = ecx->in_reliable_state;

    features::fake_ping::on_send_datagram(ecx);

    const int ret = send_datagram_original(ecx, edx, buffer);

    ecx->in_sequence_num = backup_sequence;
    ecx->in_reliable_state = backup_reliable;

    return ret;
}