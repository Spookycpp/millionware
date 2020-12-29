#include "miscellaneous.hpp"

#include "../../core/cheat.hpp"
#include "../../core/config.hpp"
#include "../../core/interfaces.hpp"
#include "../../core/patterns.hpp"

#include "../../utils/xorstr.hpp"
#include "../../utils/hash.hpp"

void features::miscellaneous::auto_accept() {
	if (!config::get<bool>(FNV_CT("misc.other.auto_accept")))
		return;

	const auto set_local_player_ready_fn = reinterpret_cast<bool(__stdcall*)(const char*)>(patterns::set_local_player_ready);

	set_local_player_ready_fn(XOR(""));
}

void features::miscellaneous::rank_reveal() {
	if (!config::get<bool>(FNV_CT("misc.other.rank_reveal")))
		return;

	// https://github.com/SteamDatabase/Protobufs/blob/master/csgo/cstrike15_usermessages.proto#L54
	interfaces::client->dispatch_user_msg(50, 0, 0, nullptr);
}
