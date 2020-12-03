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
	static const auto r_aspectratio = interfaces::convar_system->find(XOR("r_aspectratio"));

	r_aspectratio->flags &= ~CONVAR_FLAG_DEVELOPMENTONLY;
	r_aspectratio->set_value(config::get<float>(FNV("misc.other.aspect_ratio")));
}

void features::misc::clantag() {

	static float next_time = -1.f;
	static bool is_disabled = false;

	static const auto set_clantag_fn = reinterpret_cast<int(__fastcall*)(const char*, const char*)>(patterns::set_clantag);

	if (config::get<bool>(FNV_CT("misc.other.clantag"))) {

		if (interfaces::global_vars->current_time >= next_time) {
			set_clantag_fn(XOR("$ millionware"), XOR(""));

			next_time = interfaces::global_vars->current_time + 1.f;
		}

		is_disabled = false;
	}
	else if (!is_disabled)	{
		set_clantag_fn(XOR(""), XOR(""));
		is_disabled = true;
	}
}

void features::misc::name_spam() {
	if (!config::get<bool>(FNV_CT("misc.other.name_spam")))
		return;

	static const auto name = interfaces::convar_system->find(XOR("name"));
	name->callbacks.remove_all();
	name->set_value(XOR("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"));
}

void features::misc::panoramic_blur() {
	static const auto blur = interfaces::convar_system->find(XOR("@panorama_disable_blur"));
	blur->set_value(config::get<bool>(FNV_CT("misc.other.disable_panorama_blur")));
}

void features::misc::post_processing() {
	static const auto post_processing = interfaces::convar_system->find(XOR("mat_postprocess_enable"));
	post_processing->set_value(!config::get<bool>(FNV_CT("misc.other.post_processing")));
}

void features::misc::remove_flash() {
	auto local = interfaces::entity_list->get_by_index(interfaces::engine_client->get_local_player())->as_player();
	if (!local)
		return;

	local->flash_alpha() = config::get<int>(FNV_CT("misc.other.remove_flash")) ? 0.f : 255.f;
}

void features::misc::fullbright() {
	auto blur = interfaces::convar_system->find(XOR("mat_fullbright"));
	blur->set_value(!config::get<bool>(FNV_CT("misc.other.world.fullbright")) ? 0 : 1);
} 