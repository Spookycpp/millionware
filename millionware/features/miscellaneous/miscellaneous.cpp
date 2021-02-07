#include "miscellaneous.hpp"

#include "../../core/cheat.hpp"
#include "../../core/config.hpp"
#include "../../core/interfaces.hpp"
#include "../../core/patterns.hpp"

#include "../../utils/xorstr/xorstr.hpp"
#include "../../utils/hash/hash.hpp"

void features::miscellaneous::clan_tag() {

	auto set_clantag = [&](std::string tag) {
		static auto fn = reinterpret_cast<int(__fastcall*)(const char*, const char*)>(patterns::set_clantag);

		fn(tag.c_str(), STR_ENC("millionware"));
	};

	int server_time = static_cast<int>(((interfaces::global_vars->current_time / 0.296875f) + 5.60925f - 0.07f) - interfaces::engine_client->get_net_channel_info()->get_average_latency(0));

	static int last_time;
	static bool clear_tag;

	if (!config::get<bool>(FNV_CT("misc.other.clan_tag"))) {
		if (clear_tag)
			if (interfaces::global_vars->tick_count % 99 == 2) {
				set_clantag(STR_ENC(""));
				clear_tag = false;
			}
	}
	else if (interfaces::global_vars->tick_count % 48 == 2) {
		static std::string m_clantag = STR_ENC("millionware ");
		std::rotate(m_clantag.begin(), m_clantag.begin() + 1, m_clantag.end());
		std::string final_ctg = STR_ENC("$ ");
		final_ctg.append(m_clantag);

		set_clantag(final_ctg);
	}


	if (!config::get<bool>(FNV_CT("misc.other.clan_tag")))
		clear_tag = true;
}

void features::miscellaneous::auto_accept() {
	if (!config::get<bool>(FNV_CT("misc.other.auto_accept")))
		return;

	const auto set_local_player_ready_fn = reinterpret_cast<bool(__stdcall*)(const char*)>(patterns::set_local_player_ready);

	set_local_player_ready_fn(STR_ENC(""));
}

void features::miscellaneous::auto_pistol(user_cmd_t* user_cmd) {
	if (!config::get<bool>(FNV_CT("misc.other.auto_pistol")))
		return;

	if (!cheat::local_player || cheat::local_player->life_state() != LIFE_STATE_ALIVE)
		return;

	const auto weapon = (c_weapon*)cheat::local_player->active_weapon_handle().get();
	const auto info = weapon ? interfaces::weapon_system->get_weapon_info(weapon->item_definition_index()) : nullptr;

	if (!info || info->type != WEAPON_TYPE_PISTOL)
		return;

	if (user_cmd->buttons & BUTTON_IN_ATTACK2 && weapon->item_definition_index() == WEAPON_REVOLVER) {
		if (weapon->next_secondary_attack() > interfaces::global_vars->current_time)
			user_cmd->buttons &= ~BUTTON_IN_ATTACK2;
	}
	else if (user_cmd->buttons & BUTTON_IN_ATTACK && weapon->item_definition_index() != WEAPON_REVOLVER) {
		if (weapon->next_primary_attack() > interfaces::global_vars->current_time)
			user_cmd->buttons &= ~BUTTON_IN_ATTACK;
	}

}

void features::miscellaneous::rank_reveal() {
	if (!config::get<bool>(FNV_CT("misc.other.rank_reveal")))
		return;

	// https://github.com/SteamDatabase/Protobufs/blob/master/csgo/cstrike15_usermessages.proto#L54
	interfaces::client->dispatch_user_msg(50, 0, 0, nullptr);
}

void features::miscellaneous::override_fov(view_setup_t* view_setup) {

	if (cheat::local_player && !cheat::local_player->is_scoped() && cheat::local_player->life_state() == LIFE_STATE_ALIVE)
		view_setup->fov = config::get<float>(FNV_CT("misc.other.override_fov"));
}

void features::miscellaneous::panorama_blur() {
	const static auto panorama_blur = interfaces::convar_system->find(STR_ENC("@panorama_disable_blur"));

	panorama_blur->set_value(config::get<bool>(FNV_CT("visuals.other.general.panorama_blur")));
}

void features::miscellaneous::post_processing() {

	if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
		return;

	const static auto post_processing = interfaces::convar_system->find(STR_ENC("mat_postprocess_enable"));
	const static auto blur_overlay = interfaces::material_system->find_material(STR_ENC("dev/scope_bluroverlay"));
	const static auto lens_dirt = interfaces::material_system->find_material(STR_ENC("models/weapons/shared/scope/scope_lens_dirt"));

	const auto should_do_post_processing = config::get<bool>(FNV_CT("visuals.other.general.post_processing"));

	post_processing->set_value(!should_do_post_processing);
	blur_overlay->set_flag(MATERIAL_FLAG_NO_DRAW, !should_do_post_processing);
	lens_dirt->set_flag(MATERIAL_FLAG_NO_DRAW, !should_do_post_processing);
}

void features::miscellaneous::force_crosshair() {
	const static auto weapon_debug_spread_show = interfaces::convar_system->find(STR_ENC("weapon_debug_spread_show"));

	const auto should_draw_crosshair = config::get<bool>(FNV_CT("visuals.other.general.force_crosshair")) &&
		cheat::local_player && cheat::local_player->life_state() == LIFE_STATE_ALIVE && !cheat::local_player->is_scoped();

	weapon_debug_spread_show->set_value(should_draw_crosshair ? 3 : 0);
}

void features::miscellaneous::flash_alpha() {
	if (cheat::local_player == nullptr)
		return;

	cheat::local_player->flash_alpha() = config::get<float>(FNV_CT("visuals.other.general.flash_alpha"));
}

void features::miscellaneous::viewmodel_offset() {
	// @note: need convar stuff back, @czapek why u remove it?
}

void features::miscellaneous::recoil_crosshair() {
	if (!config::get<bool>(FNV_CT("visuals.other.general.recoil_crosshair")))
		return;

	// @note: need convar stuff back, @czapek why u remove it?
}

void features::miscellaneous::ragdoll_push() {

	static bool once = true;
	static auto phys_pushscale = interfaces::convar_system->find(STR_ENC("phys_pushscale"));

	if (!config::get<bool>(FNV_CT("misc.other.ragdoll_push")))
	{
		if (!once) {
			phys_pushscale->set_value(1);
		}

		once = true;
		return;
	}

	once = false;

	phys_pushscale->get_flags() |= 0;
	phys_pushscale->set_value(3000);
}

void features::miscellaneous::ragdoll_float() {
	static bool once = true;
	static auto cl_ragdoll_gravity = interfaces::convar_system->find(STR_ENC("cl_ragdoll_gravity"));

	if (!config::get<bool>(FNV_CT("misc.other.ragdoll_float")))
	{
		if (!once) {
			cl_ragdoll_gravity->set_value(600);
		}

		once = true;
		return;
	}

	once = false;

	cl_ragdoll_gravity->get_flags() |= 0;
	cl_ragdoll_gravity->set_value(-100);
}