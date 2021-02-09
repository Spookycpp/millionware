#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"

#include "../utils/util.hpp"

#include "../features/fake ping/fake_ping.hpp"

int __fastcall hooks::send_datagram_hook(c_net_channel* ecx, uintptr_t* edx, void* buffer) {
	if (buffer || !interfaces::engine_client->is_in_game() || !util::run_frame()) {
		return hooks::send_datagram.get_original<decltype(&send_datagram_hook)>()(ecx, edx, buffer);
	}

	if (interfaces::client_state->delta_tick <= 0) {
		return hooks::send_datagram.get_original<decltype(&send_datagram_hook)>()(ecx, edx, buffer);
	}

	const int backup_sequence = ecx->in_sequence_num;
	const int backup_reliable = ecx->in_reliable_state;

	features::fake_ping::on_send_datagram(ecx);

	const int ret = hooks::send_datagram.get_original<decltype(&send_datagram_hook)>()(ecx, edx, buffer);

	ecx->in_sequence_num = backup_sequence;
	ecx->in_reliable_state = backup_reliable;

	return ret;
}