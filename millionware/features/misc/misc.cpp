#include "../../core/cheat.hpp"
#include "../../core/config.hpp"
#include "../../core/interfaces.hpp"
#include "../../core/patterns.hpp"
#include "../../utils/xorstr.hpp"
#include "misc.hpp"

void features::misc::bunny_hop(user_cmd_t* user_cmd) {
	if (!config::get<bool>(FNV_CT("misc.movement.bunny_hop")))
		return;

	if (user_cmd->buttons & BUTTON_IN_JUMP && !(cheat::local_player->flags() & ENTITY_FLAG_ONGROUND))
		user_cmd->buttons &= ~BUTTON_IN_JUMP;
}

void features::misc::infinite_duck(user_cmd_t* user_cmd) {
	if (!config::get<bool>(FNV_CT("misc.movement.infinite_duck")))
		return;

	user_cmd->buttons |= BUTTON_IN_BULLRUSH;
}

void features::misc::auto_strafer(user_cmd_t* user_cmd) {
	if (!config::get<bool>(FNV_CT("misc.movement.auto_strafer")))
		return;

	//@todo: implement
}

void features::misc::auto_accept() {
	if (!config::get<bool>(FNV_CT("misc.other.auto_accept")))
		return;

	const auto set_local_player_ready_fn = reinterpret_cast<bool(__stdcall*)(const char*)>(patterns::set_local_player_ready);

	set_local_player_ready_fn(XOR(""));
	set_local_player_ready_fn(XOR("duxe"));
	set_local_player_ready_fn(XOR("is"));
	set_local_player_ready_fn(XOR("a"));
	set_local_player_ready_fn(XOR("paster"));
}

void features::misc::rank_reveal() {
	if (!config::get<bool>(FNV_CT("misc.other.rank_reveal")))
		return;

	// https://github.com/SteamDatabase/Protobufs/blob/master/csgo/cstrike15_usermessages.proto#L54
	interfaces::client->dispatch_user_msg(50, 0, 0, nullptr);
}

void features::misc::aspect_ratio() {
	//@todo: implement && convars
}
