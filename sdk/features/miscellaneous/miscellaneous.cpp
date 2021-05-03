#include <string>

#include "miscellaneous.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"

#include "../../engine/security/xorstr.h"

namespace features::miscellaneous {

    void on_create_move(c_user_cmd *user_cmd, c_weapon *weapon) {

        auto_pistol(user_cmd);
        foot_fx();
        foot_trail();
        viewmodel_offset();
    }

    void on_frame_stage_notify(const e_client_frame_stage frame_stage) {

        switch (frame_stage) {
        case e_client_frame_stage::FRAME_STAGE_NET_UPDATE_POSTDATAUPDATE_START: {
            panorama_blur();
            post_processing();

            force_crosshair();
            recoil_crosshair();
            flash_alpha();

            ragdoll_float();
            ragdoll_push();
        }
        }
    }

    void on_override_view(view_setup_t *view_setup) {
        override_fov(view_setup);
    }

    void auto_pistol(c_user_cmd *user_cmd) {

        if (!settings.miscellaneous.auto_pistol)
            return;

        // if local player is null or not alive, exit out.
        if (!cheat::local_player || cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
            return;

        // grabbing weapon entity & weapon information.
        const auto weapon = (c_weapon *) cheat::local_player->get_active_weapon_handle().get();
        const auto info = weapon ? interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index()) : nullptr;

        if (!info || info->type != WEAPON_TYPE_PISTOL)
            return;

        if (user_cmd->buttons & BUTTON_IN_ATTACK && weapon->get_item_definition_index() == WEAPON_REVOLVER) {
            if (weapon->get_next_secondary_attack() > interfaces::global_vars->current_time)
                user_cmd->buttons &= ~BUTTON_IN_ATTACK2;
        }
        else if (user_cmd->buttons & BUTTON_IN_ATTACK && weapon->get_item_definition_index() != WEAPON_REVOLVER) {
            if (weapon->get_next_primary_attack() > interfaces::global_vars->current_time)
                user_cmd->buttons &= ~BUTTON_IN_ATTACK;
        }
    }

    void auto_accept() {
        if (!settings.miscellaneous.auto_accept)
            return;

        const auto set_local_player_ready_fn = reinterpret_cast<bool(__stdcall *)(const char *)>(patterns::accept_match);

        set_local_player_ready_fn(XORSTR(""));
    }

    void rank_reveal(c_user_cmd *user_cmd) {
        if (!settings.miscellaneous.rank_reveal)
            return;

        if (!(user_cmd->buttons & BUTTON_IN_SCORE))
            return;

        // https://github.com/SteamDatabase/Protobufs/blob/master/csgo/cstrike15_usermessages.proto#L54
        interfaces::client_dll->dispatch_user_msg(50, 0, 0, nullptr);
    }

    void override_fov(view_setup_t *view_setup) {
        if (cheat::local_player && !cheat::local_player->get_is_scoped() && cheat::local_player->get_life_state() == LIFE_STATE_ALIVE)
            view_setup->fov = (float) settings.visuals.local.override_fov;
    }

    void post_processing() {

        if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
            return;

        const static auto post_processing = interfaces::convar_system->find_convar(XORSTR("mat_postprocess_enable"));
        const static auto blur_overlay = interfaces::material_system->find_material(XORSTR("dev/scope_bluroverlay"));
        const static auto lens_dirt = interfaces::material_system->find_material(XORSTR("models/weapons/shared/scope/scope_lens_dirt"));

        const auto should_do_post_processing = settings.visuals.local.disable_post_processing;

        post_processing->set_value(!should_do_post_processing);
        blur_overlay->set_flag(MATERIAL_FLAG_NO_DRAW, !should_do_post_processing);
        lens_dirt->set_flag(MATERIAL_FLAG_NO_DRAW, !should_do_post_processing);
    }

    void panorama_blur() {
        const static auto panorama_blur = interfaces::convar_system->find_convar(XORSTR("@panorama_disable_blur"));

        panorama_blur->set_value(settings.visuals.local.disable_panorama_blur);
    }

    void force_crosshair() {
        const static auto weapon_debug_spread_show = interfaces::convar_system->find_convar(XORSTR("weapon_debug_spread_show"));

        const auto should_draw_crosshair =
            settings.visuals.local.sniper_crosshair && cheat::local_player && cheat::local_player->get_life_state() == LIFE_STATE_ALIVE && !cheat::local_player->get_is_scoped();

        weapon_debug_spread_show->set_value(should_draw_crosshair ? 3 : 0);
    }

    void flash_alpha() {
        if (!cheat::local_player)
            return;

        cheat::local_player->get_flash_alpha() = 255.0f * settings.visuals.local.flash_alpha / 100.0f;
    }

    void recoil_crosshair() {
        const static auto recoil_crosshair = interfaces::convar_system->find_convar(XORSTR("cl_crosshair_recoil"));

        recoil_crosshair->set_value(settings.visuals.local.recoil_crosshair);
    }

    void ragdoll_push() {
        const static auto phys_pushscale = interfaces::convar_system->find_convar(XORSTR("phys_pushscale"));

        if (settings.miscellaneous.ragdoll_push) {
            phys_pushscale->get_flags() |= 0;
            phys_pushscale->set_value(3000);
        }
        else {
            phys_pushscale->set_value(1);
        }
    }

    void ragdoll_float() {
        const static auto cl_ragdoll_gravity = interfaces::convar_system->find_convar(XORSTR("cl_ragdoll_gravity"));

        if (settings.miscellaneous.ragdoll_float) {
            cl_ragdoll_gravity->get_flags() |= 0;
            cl_ragdoll_gravity->set_value(-100);
        }
        else {
            cl_ragdoll_gravity->set_value(600);
        }
    }

    void skybox_changer(const int skybox) {
        if (!interfaces::engine_client->is_in_game())
            return;

        static auto sv_skyname = interfaces::convar_system->find_convar(XORSTR("sv_skyname"));

        if (!sv_skyname)
            return;

        static auto r_3dsky = interfaces::convar_system->find_convar(XORSTR("r_3dsky"));

        if (!r_3dsky)
            return;

        std::string skybox_name;

        switch (skybox) {
			case 1:  skybox_name = XORSTR("cs_tibet");					break;
			case 2:  skybox_name = XORSTR("cs_baggage_skybox_");		break;
			case 3:  skybox_name = XORSTR("embassy");					break;
			case 4:  skybox_name = XORSTR("italy");						break;
			case 5:  skybox_name = XORSTR("jungle");					break;
			case 6:  skybox_name = XORSTR("office");					break;
			case 7:  skybox_name = XORSTR("sky_cs15_daylight01_hdr");	break;
			case 8:  skybox_name = XORSTR("vertigoblue_hdr");			break;
			case 9:  skybox_name = XORSTR("sky_cs15_daylight02_hdr");	break;
			case 10: skybox_name = XORSTR("vertigo");					break;
			case 11: skybox_name = XORSTR("sky_day02_05_hdr");			break;
			case 12: skybox_name = XORSTR("nukeblank");					break;
			case 13: skybox_name = XORSTR("sky_venice");				break;
			case 14: skybox_name = XORSTR("sky_cs15_daylight03_hdr");	break;
			case 15: skybox_name = XORSTR("sky_cs15_daylight04_hdr");	break;
			case 16: skybox_name = XORSTR("sky_csgo_cloudy01");			break;
			case 17: skybox_name = XORSTR("sky_csgo_night02");			break;
			case 18: skybox_name = XORSTR("sky_csgo_night02b");			break;
			case 19: skybox_name = XORSTR("sky_csgo_night_flat");		break;
			case 20: skybox_name = XORSTR("sky_dust");					break;
			case 21: skybox_name = XORSTR("vietnam");					break;
			case 22: skybox_name = XORSTR("custom");					break;
			default: skybox_name = sv_skyname->get_string();			break;
		}

        if (skybox_name.empty())
            return;

        static auto load_named_sky_addr = patterns::load_named_sky;
        static auto load_named_sky_fn = reinterpret_cast<void(__fastcall *)(const char *)>(load_named_sky_addr);

        if (!load_named_sky_fn)
            return;

        load_named_sky_fn(skybox_name.c_str());

        r_3dsky->set_value(skybox != 0 ? 0 : 1);
    }

    void foot_fx() {
        if (cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
            return;

        if (interfaces::engine_client->is_in_game() && !interfaces::engine_client->is_connected())
            return;

        switch (settings.visuals.local.feet_fx) {
        case 0:
            break;
        case 1:
            interfaces::effects->sparks(cheat::local_player->get_vec_origin());
            break;
        case 2:
            interfaces::effects->dust(cheat::local_player->get_vec_origin(), cheat::local_player->get_velocity() * interfaces::global_vars->interval_per_tick * 0.5f, 1.f,
                                      cheat::local_player->get_velocity().length_2d() / 250.f);
            break;
        case 3:
            interfaces::effects->energy_splash(cheat::local_player->get_vec_origin(), cheat::local_player->get_velocity() * interfaces::global_vars->interval_per_tick * 0.2f, true);
            break;
        }
    }

    void foot_trail() {

        if (cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
            return;

        if (interfaces::engine_client->is_in_game() && !interfaces::engine_client->is_connected())
            return;
    }

    void viewmodel_offset() {

        // hash
        static float last_hash = 0.f;

        // finding original values of viewmodel x, y & z
        static float og_x = 0;
        static float og_y = 0;
        static float og_z = 0;

        if (!og_x || !og_y || !og_z) {
            og_x = interfaces::convar_system->find_convar(XORSTR("viewmodel_offset_x"))->get_float();
            og_y = interfaces::convar_system->find_convar(XORSTR("viewmodel_offset_y"))->get_float();
            og_z = interfaces::convar_system->find_convar(XORSTR("viewmodel_offset_z"))->get_float();
        }

        static bool is_disabled = false;
        if (settings.visuals.local.viewmodel_offset) {
            float cur_hash = settings.visuals.local.viewmodel_offset_x + settings.visuals.local.viewmodel_offset_y + settings.visuals.local.viewmodel_offset_z;
            if (last_hash != cur_hash) {
                // setting viewmodel to the menu element value
                interfaces::convar_system->find_convar(XORSTR("viewmodel_offset_x"))->set_value(settings.visuals.local.viewmodel_offset_x);
                interfaces::convar_system->find_convar(XORSTR("viewmodel_offset_y"))->set_value(settings.visuals.local.viewmodel_offset_y);
                interfaces::convar_system->find_convar(XORSTR("viewmodel_offset_z"))->set_value(settings.visuals.local.viewmodel_offset_z);
                is_disabled = false;
                last_hash = cur_hash;
            }
        }
        else {
            if (!is_disabled) {
                // restoring original xyz
                interfaces::convar_system->find_convar(XORSTR("viewmodel_offset_x"))->set_value(og_x);
                interfaces::convar_system->find_convar(XORSTR("viewmodel_offset_y"))->set_value(og_y);
                interfaces::convar_system->find_convar(XORSTR("viewmodel_offset_z"))->set_value(og_z);
                is_disabled = true;
            }
        }
    }

} // namespace features::miscellaneous