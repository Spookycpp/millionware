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
}

void features::misc::rank_reveal() {
	if (!config::get<bool>(FNV_CT("misc.other.rank_reveal")))
		return;

	// https://github.com/SteamDatabase/Protobufs/blob/master/csgo/cstrike15_usermessages.proto#L54
	interfaces::client->dispatch_user_msg(50, 0, 0, nullptr);
}

void features::misc::clantag() {
	static auto is_disabled = false;
	static auto next_time = -1.0f;

	static const auto set_clantag_fn = reinterpret_cast<int(__fastcall*)(const char*, const char*)>(patterns::set_clantag);

	if (config::get<bool>(FNV_CT("misc.other.clantag"))) {
		if (interfaces::global_vars->current_time >= next_time) {
			set_clantag_fn(XOR("$ millionware"), XOR(""));
			next_time = interfaces::global_vars->current_time + 1.f;
		}

		is_disabled = false;
	}
	else if (!is_disabled) {
		set_clantag_fn(XOR(""), XOR(""));
		is_disabled = true;
	}
}

void features::misc::name_spam() {
	if (!config::get<bool>(FNV_CT("misc.other.name_spam")))
		return;

	static const auto name = interfaces::convar_system->find(XOR("name"));

	name->callbacks.element_count(0);
	name->set_value(XOR("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"));
}

void features::misc::remove_panorama_blur() {
	static const auto panorama_disable_blur = interfaces::convar_system->find(XOR("@panorama_disable_blur"));

	panorama_disable_blur->set_value(config::get<bool>(FNV_CT("misc.other.remove_panorama_blur")));
}

void features::misc::remove_post_processing() {
	static const auto mat_postprocess_enable = interfaces::convar_system->find(XOR("mat_postprocess_enable"));

	mat_postprocess_enable->set_value(!config::get<bool>(FNV_CT("misc.other.remove_post_processing")));
}

void features::misc::flash_alpha() {
	if (cheat::local_player == nullptr)
		return;
	
	cheat::local_player->flash_alpha() = config::get<float>(FNV_CT("misc.other.flash_alpha"));
}

void features::misc::override_fov(view_setup_t* view_setup) {
	if (cheat::local_player == nullptr)
		return;

	if (!cheat::local_player->is_scoped()) {
		view_setup->fov = config::get<float>(FNV_CT("misc.other.override_fov"));
	}
}

void features::misc::force_crosshair() {
	static const auto weapon_debug_spread_show = interfaces::convar_system->find(XOR("weapon_debug_spread_show"));

	// @todo: fix for spectators
	const auto should_force = config::get<bool>(FNV_CT("misc.other.force_crosshair")) &&
		cheat::local_player->life_state() == LIFE_STATE_ALIVE && !cheat::local_player->is_scoped();

	weapon_debug_spread_show->set_value(should_force ? 3 : 0);
}

void features::misc::unlock_hidden_convars() {
	if (!config::get<bool>(FNV_CT("misc.other.unlock_hidden_convars")))
		return;

	auto it = *reinterpret_cast<c_convar**>((uint32_t)interfaces::convar_system + 0x34);
	for (c_convar* cvar = it->next; cvar != nullptr; cvar = cvar->next) {
		cvar->flags &= ~CONVAR_FLAG_HIDDEN;
		cvar->flags &= ~CONVAR_FLAG_DEVELOPMENTONLY;
	}
}

void features::misc::third_person() {
	if (!config::get<bool>(FNV_CT("misc.other.third_person")))
		return;

	if (!interfaces::engine_client->is_in_game())
		return;

	bool alive = cheat::local_player && cheat::local_player->life_state() == LIFE_STATE_ALIVE;

	if (alive /* once we add a hotkey system ill finish this */) {
		if (!interfaces::input->camera_in_third_person) {
			interfaces::input->camera_in_third_person = true;
			interfaces::input->camera_offset.z = 150.f;
		}
	}

	else if (interfaces::input->camera_in_third_person) {
		interfaces::input->camera_in_third_person = false;
		interfaces::input->camera_offset.z = 0.f;
	}
}

void features::misc::auto_pistol(user_cmd_t* user_cmd) {
	if (!config::get<bool>(FNV_CT("misc.other.auto_pistol")))
		return;

	const auto weapon = cheat::local_player->active_weapon_handle().get()->as_weapon();
	if (!weapon)
		return;

	const auto weapon_info = interfaces::weapon_system->get_weapon_info(weapon->item_definition_index());
	if (weapon_info->full_auto)
		return;

	const auto next_planned_attack = weapon->next_primary_attack() + config::get<float>(FNV_CT("misc.other.auto_pistol_delay"));

	if (user_cmd->buttons & BUTTON_IN_ATTACK && next_planned_attack >= interfaces::global_vars->current_time)
		user_cmd->buttons &= ~BUTTON_IN_ATTACK;
}