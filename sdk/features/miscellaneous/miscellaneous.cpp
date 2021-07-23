#include <string>

#include "miscellaneous.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"
#include "../../core/util/util.h"

#include "../../engine/logging/logging.h"
#include "../../engine/math/math.h"
#include "../../engine/security/xorstr.h"

namespace features::miscellaneous {

    void on_create_move(c_user_cmd *user_cmd, c_weapon *weapon) {
        auto_pistol(user_cmd);
        clantag();
        foot_fx();
        foot_trail();
        viewmodel_offset();
    }

    void on_frame_stage_notify(const e_client_frame_stage frame_stage) {
        switch (frame_stage) {
            case e_client_frame_stage::FRAME_STAGE_NET_UPDATE_POSTDATAUPDATE_START: {
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

        if (!cheat::local_player || cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
            return;

        const auto weapon = (c_weapon *) cheat::local_player->get_active_weapon_handle().get();
        const auto info = weapon ? interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index()) : nullptr;

        if (!info || info->type != WEAPON_TYPE_PISTOL)
            return;

        if (user_cmd->buttons & BUTTON_IN_ATTACK && weapon->get_item_definition_index() == WEAPON_REVOLVER) {
            if (weapon->get_next_secondary_attack() > cheat::local_player->get_tick_base() * interfaces::global_vars->interval_per_tick)
                user_cmd->buttons &= ~BUTTON_IN_ATTACK2;
        }
        else if (user_cmd->buttons & BUTTON_IN_ATTACK && weapon->get_item_definition_index() != WEAPON_REVOLVER) {
            if (weapon->get_next_primary_attack() > cheat::local_player->get_tick_base() * interfaces::global_vars->interval_per_tick)
                user_cmd->buttons &= ~BUTTON_IN_ATTACK;
        }
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

    void clantag() {
        if (!interfaces::engine_client->is_in_game())
            return;

        auto set_clantag = [&](std::string tag) { // fps enhancer
            static auto set_tag_fn = reinterpret_cast<int(__fastcall *)(const char *, const char *)>(patterns::get_set_clantag());

            set_tag_fn(tag.c_str(), XORSTR("millionware"));
        };

        if (!interfaces::engine_client->get_net_channel_info())
            return;

        static bool clear_tag = false;
        static int tick_count = 0;

        int server_time = static_cast<int>(((interfaces::global_vars->current_time / 0.296875f) + 5.60925f - 0.07f) - interfaces::engine_client->get_net_channel_info()->get_average_latency(0));

        if (!settings.miscellaneous.clantag && clear_tag) {
            if (interfaces::global_vars->tick_count % 99 == 2) {
                set_clantag(XORSTR(""));
                clear_tag = false;
            }
        }
        else if (settings.miscellaneous.clantag && server_time != tick_count) {
            static std::string clantag = XORSTR("millionware ");
            std::rotate(clantag.begin(), clantag.begin() + 1, clantag.end());
            std::string prefix = XORSTR("$ ");
            prefix.append(clantag);

            set_clantag(prefix);
            tick_count = server_time;
        }

        if (!settings.miscellaneous.clantag)
            clear_tag = true;
    }

    void post_processing() {
        if (!interfaces::engine_client->is_in_game())
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
            phys_pushscale->callbacks.clear();
            phys_pushscale->set_value(3000);
        }
        else {
            phys_pushscale->set_value(1);
        }
    }

    void ragdoll_float() {
        const static auto cl_ragdoll_gravity = interfaces::convar_system->find_convar(XORSTR("cl_ragdoll_gravity"));

        if (settings.miscellaneous.ragdoll_float) {
            cl_ragdoll_gravity->callbacks.clear();
            cl_ragdoll_gravity->set_value(-100);
        }
        else {
            cl_ragdoll_gravity->set_value(600);
        }
    }

    void name_spam() {
        if (!interfaces::engine_client->is_in_game())
            return;

        static auto name = interfaces::convar_system->find_convar(XORSTR("name"));

        name->callbacks.clear();

        char buffer[129];

        memset(buffer, 0, 129);
        memset(buffer, '$', 128);

        name->set_value(buffer);
    }

    void server_selector() {
        //const char *regions[] = {
        //    (""),    ("syd"), ("vie"), ("gru"), ("scl"),  ("dxb"), ("par"), ("fra"), ("hkg"), ("maa"),
        //    ("bom"), ("tyo"), ("lux"), ("ams"), ("limc"), ("man"), ("waw"), ("sgp"), ("jnb"), ("mad"),
        //    ("sto"), ("lhr"), ("atl"), ("ord"), ("lax"),  ("mwh"), ("okc"), ("sea"), ("iad")};

        //static std::string *relay_cluster = *(std::string **) (patterns::get_pattern("steamnetworkingsockets.dll", "B8 ? ? ? ? B9 ? ? ? ? 0F 43") + 1);
        //*relay_cluster = "lax";
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

        // clang-format off
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
			default: skybox_name = sv_skyname->string;			        break;
		}
        // clang-format on

        if (skybox_name.empty())
            return;

        static auto load_named_sky_addr = patterns::get_load_named_sky();
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

        // clang-format off
        switch (settings.visuals.local.feet_fx) {
        case 0: break;
        case 1: interfaces::effects->sparks(cheat::local_player->get_vec_origin()); break;
        case 2: interfaces::effects->dust(cheat::local_player->get_vec_origin(), cheat::local_player->get_velocity() * interfaces::global_vars->interval_per_tick * 0.5f, 1.f, cheat::local_player->get_velocity().length_2d() / 250.f); break;
        case 3: interfaces::effects->energy_splash(cheat::local_player->get_vec_origin(), cheat::local_player->get_velocity() * interfaces::global_vars->interval_per_tick * 0.2f, true); break;
        }
        // clang-format on
    }

    void foot_trail() {
        if (cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
            return;

        if (interfaces::engine_client->is_in_game() && !interfaces::engine_client->is_connected())
            return;

        static bool jump_failed = false;
        static int tick, ignoreticks = 1, old_velocity;
        static vector_t old_origin;

        if (!settings.visuals.local.foot_trail) {
            old_origin = cheat::local_player->get_vec_origin();
            return;
        }

        if (!tick) {
            beam_info_t beam_info;
            beam_info.m_ntype = 0;
            beam_info.m_pszmodelname = XORSTR("sprites/purplelaser1.vmt");
            beam_info.m_nmodelindex = interfaces::model_info->get_model_index(XORSTR("sprites/purplelaser1.vmt"));
            beam_info.m_flhaloscale = 0.0;
            beam_info.m_fllife = settings.visuals.local.trail_time;
            beam_info.m_flwidth = settings.visuals.local.trail_size;
            beam_info.m_flendwidth = settings.visuals.local.trail_size;
            beam_info.m_flfadelength = 0.0;
            beam_info.m_flamplitude = 2.0;
            beam_info.m_flspeed = 0.5;
            beam_info.m_flbrightness = 255.f;
            beam_info.m_nstartframe = 0;
            beam_info.m_flframerate = 0;

            beam_info.m_flred = (float) settings.visuals.local.trail_color.r / 255.0f;
            beam_info.m_flgreen = (float) settings.visuals.local.trail_color.g / 255.0f;
            beam_info.m_flblue = (float) settings.visuals.local.trail_color.b / 255.0f;

            beam_info.m_nsegments = 2;
            beam_info.m_brenderable = true;
            beam_info.m_nflags = 0;
            beam_info.m_vecstart = old_origin;
            beam_info.m_vecend = cheat::local_player->get_vec_origin();

            auto beam = interfaces::render_beams->create_beam(beam_info);
            if (beam)
                interfaces::render_beams->draw_beam(beam);

            old_velocity = (int) cheat::local_player->get_velocity().length_2d();
        }
    }

    static uintptr_t *death_notice = nullptr;

    void preserve_killfeed() {

        if (auto game_rules = c_game_rules::get(); !game_rules || game_rules->get_freeze_period()) {
            death_notice = 0;
            return;
        }

        if (!cheat::local_player || cheat::local_player->get_life_state() != LIFE_STATE_ALIVE) {
            death_notice = 0;
            return;
        }

        if (!death_notice)
            death_notice = util::find_hud_element(XORSTR("CCSGO_HudDeathNotice"));

        if (death_notice) {
            auto local_death_notice = (float *) ((uintptr_t) death_notice + 0x50);

            if (local_death_notice)
                *local_death_notice = settings.miscellaneous.preserve_killfeed ? FLT_MAX : 1.5f;

            if (cheat::should_clear_death_notices) {

                cheat::should_clear_death_notices = false;

                static auto clear_notices = (void(__thiscall *)(uintptr_t)) patterns::get_clear_death_notices();

                clear_notices((uintptr_t) death_notice - 0x14);
            }
        }
    }

    void unlock_hidden_convars() {
        if (!interfaces::convar_system)
            return;

        auto p = **reinterpret_cast<c_convar ***>(interfaces::convar_system + 0x34);
        for (auto c = p->next; c != nullptr; c = c->next) {
            c->flags &= ~CVAR_DEVELOPMENT_ONLY;
            c->flags &= ~CVAR_HIDDEN;
        }
    }

    void viewmodel_offset() {
        static c_convar *viewmodel_offset_x = nullptr;
        static c_convar *viewmodel_offset_y = nullptr;
        static c_convar *viewmodel_offset_z = nullptr;

        static auto last_state = settings.visuals.local.viewmodel_offset;
        static auto old_x = 0.0f;
        static auto old_y = 0.0f;
        static auto old_z = 0.0f;

        if (viewmodel_offset_x == nullptr) {
            viewmodel_offset_x = interfaces::convar_system->find_convar(XORSTR("viewmodel_offset_x"));
            viewmodel_offset_y = interfaces::convar_system->find_convar(XORSTR("viewmodel_offset_y"));
            viewmodel_offset_z = interfaces::convar_system->find_convar(XORSTR("viewmodel_offset_z"));

            viewmodel_offset_x->callbacks.clear();
            viewmodel_offset_y->callbacks.clear();
            viewmodel_offset_z->callbacks.clear();
        }

        if (settings.visuals.local.viewmodel_offset != last_state) {
            if (settings.visuals.local.viewmodel_offset) {
                old_x = viewmodel_offset_x->get_float();
                old_y = viewmodel_offset_y->get_float();
                old_z = viewmodel_offset_z->get_float();
            }
            else {
                viewmodel_offset_x->set_value(old_x);
                viewmodel_offset_y->set_value(old_y);
                viewmodel_offset_z->set_value(old_z);
            }

            last_state = settings.visuals.local.viewmodel_offset;
        }

        if (!settings.visuals.local.viewmodel_offset)
            return;

        viewmodel_offset_x->set_value(settings.visuals.local.viewmodel_offset_x);
        viewmodel_offset_y->set_value(settings.visuals.local.viewmodel_offset_y);
        viewmodel_offset_z->set_value(settings.visuals.local.viewmodel_offset_z);
    }

} // namespace features::miscellaneous