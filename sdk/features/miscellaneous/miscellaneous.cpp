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
        viewmodel_offset();
    }

    void on_frame_stage_notify(const e_client_frame_stage frame_stage) {
        switch (frame_stage) {
        // clang-format off
            case e_client_frame_stage::FRAME_STAGE_NET_UPDATE_POSTDATAUPDATE_START: {
                post_processing();

                force_crosshair();
                recoil_crosshair();
                flash_alpha();

                ragdoll_float();
                ragdoll_push();

                remove_smoke();
            }
            // clang-format on
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
        } else if (user_cmd->buttons & BUTTON_IN_ATTACK && weapon->get_item_definition_index() != WEAPON_REVOLVER) {
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

        if (settings.visuals.local.override_fov == 90)
            return;

        if (!cheat::local_player)
            return;

        if (cheat::local_player->get_life_state() != LIFE_STATE_ALIVE) {
            auto obs_mode = cheat::local_player->get_observer_mode();
            auto obs_target = (c_player *) cheat::local_player->get_observer_target().get();

            if ((obs_mode == OBS_MODE_IN_EYE || obs_mode == OBS_MODE_DEATHCAM) && !obs_target->get_is_scoped())
                view_setup->fov = (float) settings.visuals.local.override_fov;
        } else if (!cheat::local_player->get_is_scoped()) {
            view_setup->fov = (float) settings.visuals.local.override_fov;
        }
    }

    void clantag() {
        if (!interfaces::engine_client->is_in_game())
            return;

        auto set_clantag = [&](std::string tag) { // fps enhancer
            static auto set_tag_fn = reinterpret_cast<int(__fastcall *)(const char *, const char *)>(patterns::get_set_clantag());

            set_tag_fn(tag.c_str(), xs("millionware"));
        };

        if (!interfaces::engine_client->get_net_channel_info())
            return;

        static auto cl_clanid = interfaces::convar_system->find_convar(xs("cl_clanid"));
        static int cl_clanid_value = cl_clanid->get_int();

        // doing it this way because doing set_value just didn't work so, fuck it.
        auto tag = std::format(xs("cl_clanid {}"), cl_clanid_value);

        static bool reset_tag = false;
        static int last_time = 0;

        // account for server latency.
        int server_time = static_cast<int>(((interfaces::global_vars->current_time / 0.296875f) + 5.60925f - 0.07f) -
                                           interfaces::engine_client->get_net_channel_info()->get_average_latency(0));

        if (!settings.miscellaneous.clantag && reset_tag) {
            if (interfaces::global_vars->tick_count % 99 == 2) {
                interfaces::engine_client->execute_command(tag.c_str());
                reset_tag = false;
            }
        } else if (server_time != last_time) {
            static std::string clantag = xs("millionware ");

            std::rotate(clantag.begin(), clantag.begin() + 1, clantag.end());

            set_clantag(clantag);
            last_time = server_time;
        }

        if (!settings.miscellaneous.clantag)
            reset_tag = true;
    }

    void post_processing() {
        if (!interfaces::engine_client->is_in_game())
            return;

        const static auto post_processing = interfaces::convar_system->find_convar(xs("mat_postprocess_enable"));
        const static auto blur_overlay = interfaces::material_system->find_material(xs("dev/scope_bluroverlay"));
        const static auto lens_dirt = interfaces::material_system->find_material(xs("models/weapons/shared/scope/scope_lens_dirt"));

        const auto should_do_post_processing = settings.visuals.local.disable_post_processing;

        post_processing->set_value(!should_do_post_processing);
        blur_overlay->set_flag(MATERIAL_FLAG_NO_DRAW, !should_do_post_processing);
        lens_dirt->set_flag(MATERIAL_FLAG_NO_DRAW, !should_do_post_processing);
    }

    void panorama_blur() {
        const static auto panorama_blur = interfaces::convar_system->find_convar(xs("@panorama_disable_blur"));

        panorama_blur->set_value(settings.visuals.local.disable_panorama_blur);
    }

    void force_crosshair() {
        const static auto weapon_debug_spread_show = interfaces::convar_system->find_convar(xs("weapon_debug_spread_show"));

        const auto should_draw_crosshair = settings.visuals.local.sniper_crosshair && cheat::local_player &&
                                           cheat::local_player->get_life_state() == LIFE_STATE_ALIVE &&
                                           !cheat::local_player->get_is_scoped();

        weapon_debug_spread_show->set_value(should_draw_crosshair ? 3 : 0);
    }

    void flash_alpha() {
        if (!cheat::local_player)
            return;

        cheat::local_player->get_flash_alpha() = 255.0f * settings.visuals.local.flash_alpha / 100.0f;
    }

    void recoil_crosshair() {
        const static auto recoil_crosshair = interfaces::convar_system->find_convar(xs("cl_crosshair_recoil"));

        // lord forgive me
        if (settings.visuals.local.recoil_crosshair == 0) {
            recoil_crosshair->set_value(0);
        } else if (settings.visuals.local.recoil_crosshair == 1) {
            recoil_crosshair->set_value(1);
        } else if (settings.visuals.local.recoil_crosshair == 2) {
            recoil_crosshair->set_value(0);
        }
    }

    void ragdoll_push() {
        const static auto phys_pushscale = interfaces::convar_system->find_convar(xs("phys_pushscale"));

        if (settings.miscellaneous.ragdoll_push) {
            phys_pushscale->callbacks.clear();
            phys_pushscale->set_value(3000);
        } else {
            phys_pushscale->set_value(1);
        }
    }

    void ragdoll_float() {
        const static auto cl_ragdoll_gravity = interfaces::convar_system->find_convar(xs("cl_ragdoll_gravity"));

        if (settings.miscellaneous.ragdoll_float) {
            cl_ragdoll_gravity->callbacks.clear();
            cl_ragdoll_gravity->set_value(-100);
        } else {
            cl_ragdoll_gravity->set_value(600);
        }
    }

    void name_spam() {
        if (!interfaces::engine_client->is_in_game())
            return;

        static auto name = interfaces::convar_system->find_convar(xs("name"));

        name->callbacks.clear();

        char buffer[129];

        memset(buffer, 0, 129);
        memset(buffer, '$', 128);

        name->set_value(buffer);
    }

    void server_selector() { // https://www.unknowncheats.me/forum/counterstrike-global-offensive/381959-forcing-datacenter.html

        if (interfaces::engine_client->is_in_game())
            return;

        const char *regions[] = {
            xs(""),    xs("syd"), xs("vie"), xs("gru"), xs("scl"),  xs("dxb"), xs("par"), xs("fra"), xs("hkg"), xs("maa"),
            xs("bom"), xs("tyo"), xs("lux"), xs("ams"), xs("limc"), xs("man"), xs("waw"), xs("sgp"), xs("jnb"), xs("mad"),
            xs("sto"), xs("lhr"), xs("atl"), xs("eat"), xs("ord"),  xs("lax"), xs("mwh"), xs("okc"), xs("sea"), xs("iad")
        };

        static std::string *relay_cluster = *(std::string **) (patterns::get_relay_cluster() + 1);
        *relay_cluster = regions[settings.miscellaneous.server_regions];
    }

    void skybox_changer(const int skybox) {

        // kek
        if (settings.visuals.world.skybox == 0 || !interfaces::engine_client->is_in_game())
            return;

        static auto sv_skyname = interfaces::convar_system->find_convar(xs("sv_skyname"));

        if (!sv_skyname)
            return;

        static auto r_3dsky = interfaces::convar_system->find_convar(xs("r_3dsky"));

        if (!r_3dsky)
            return;

        std::string skybox_name;

        // clang-format off
        switch (skybox) {
			case 1:  skybox_name = xs("cs_tibet");					break;
			case 2:  skybox_name = xs("cs_baggage_skybox_");		break;
			case 3:  skybox_name = xs("embassy");					break;
			case 4:  skybox_name = xs("italy");						break;
			case 5:  skybox_name = xs("jungle");					break;
			case 6:  skybox_name = xs("office");					break;
			case 7:  skybox_name = xs("sky_cs15_daylight01_hdr");	break;
			case 8:  skybox_name = xs("vertigoblue_hdr");			break;
			case 9:  skybox_name = xs("sky_cs15_daylight02_hdr");	break;
			case 10: skybox_name = xs("vertigo");					break;
			case 11: skybox_name = xs("sky_day02_05_hdr");			break;
			case 12: skybox_name = xs("nukeblank");					break;
			case 13: skybox_name = xs("sky_venice");				break;
			case 14: skybox_name = xs("sky_cs15_daylight03_hdr");	break;
			case 15: skybox_name = xs("sky_cs15_daylight04_hdr");	break;
			case 16: skybox_name = xs("sky_csgo_cloudy01");			break;
			case 17: skybox_name = xs("sky_csgo_night02");			break;
			case 18: skybox_name = xs("sky_csgo_night02b");			break;
			case 19: skybox_name = xs("sky_csgo_night_flat");		break;
			case 20: skybox_name = xs("sky_dust");					break;
			case 21: skybox_name = xs("vietnam");					break;
			case 22: skybox_name = xs("custom");					break;
			default: skybox_name = sv_skyname->string;			    break;
		}
        // clang-format on

        if (skybox_name.empty())
            return;

        static auto load_named_sky_addr = patterns::get_load_named_sky();
        static auto load_named_sky_fn = reinterpret_cast<void(__fastcall *)(const char *)>(load_named_sky_addr);

        if (!load_named_sky_fn)
            return;

        const int skybox_type = settings.visuals.world.skybox;

        static int last_state = skybox_type;

        if (last_state != skybox_type || cheat::set_skybox == true) {

            load_named_sky_fn(skybox_name.c_str());

            r_3dsky->set_value(skybox != 0 ? 0 : 1);

            last_state = settings.visuals.world.skybox;
            cheat::set_skybox = false;
        }
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

    void preserve_killfeed() {

        if (!interfaces::engine_client->is_in_game())
            return;

        static auto *death_notice = util::find_hud_element(xs("CCSGO_HudDeathNotice"));
        static auto clear_notices = (void(__thiscall *)(uintptr_t)) patterns::get_clear_death_notices();

        // note - laine;
        // only doing spawn_time check because in warmup
        // obviously the round_start event won't occur
        static float last_spawntime = cheat::local_player->spawn_time();

        if (last_spawntime != cheat::local_player->spawn_time() || cheat::reset_killfeed == true) {
            death_notice = util::find_hud_element(xs("CCSGO_HudDeathNotice"));

            if (death_notice)
                clear_notices(((uintptr_t) death_notice - 0x14));

            last_spawntime = cheat::local_player->spawn_time();
            cheat::reset_killfeed = false;
        }

        if (death_notice && cheat::local_player->is_alive())
            *(float *) ((uintptr_t) death_notice + 0x50) = settings.miscellaneous.preserve_killfeed ? FLT_MAX : 1.5f;
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
            viewmodel_offset_x = interfaces::convar_system->find_convar(xs("viewmodel_offset_x"));
            viewmodel_offset_y = interfaces::convar_system->find_convar(xs("viewmodel_offset_y"));
            viewmodel_offset_z = interfaces::convar_system->find_convar(xs("viewmodel_offset_z"));

            viewmodel_offset_x->callbacks.clear();
            viewmodel_offset_y->callbacks.clear();
            viewmodel_offset_z->callbacks.clear();
        }

        if (settings.visuals.local.viewmodel_offset != last_state) {
            if (settings.visuals.local.viewmodel_offset) {
                old_x = viewmodel_offset_x->get_float();
                old_y = viewmodel_offset_y->get_float();
                old_z = viewmodel_offset_z->get_float();
            } else {
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

    void remove_smoke() {
        // used for removing the smoke overlay once you're inside of it.
        static uint32_t *smoke_count = nullptr;
        if (!smoke_count)
            smoke_count = *reinterpret_cast<uint32_t **>(patterns::get_smoke_count() + 1);

        // if smoke_count is nullptr or client is not ingame, exit out.
        if (!smoke_count || !interfaces::engine_client->is_in_game())
            return;

        // obtain material we desire to remove.
        const auto material = interfaces::material_system->find_material(xs("particle\\vistasmokev1\\vistasmokev1_smokegrenade"));

        // obtain what value to set smoke to.
        const int smoke_type = settings.visuals.world.smoke_type;

        // ensure the code is only ran once.
        static int last_state = smoke_type;

        if (last_state != smoke_type) {
            switch (smoke_type) {
            case 0: { // no smoke change.
                material->set_flag(MATERIAL_FLAG_WIREFRAME, false);
                material->set_flag(MATERIAL_FLAG_NO_DRAW, false);
                *(int *) smoke_count = 1;
                break;
            }
            case 1: { // wireframe smoke.
                material->set_flag(MATERIAL_FLAG_WIREFRAME, true);
                material->set_flag(MATERIAL_FLAG_NO_DRAW, false);
                *(int *) smoke_count = 0;
                break;
            }
            case 2: { // remove smoke.
                material->set_flag(MATERIAL_FLAG_WIREFRAME, false);
                material->set_flag(MATERIAL_FLAG_NO_DRAW, true);
                *(int *) smoke_count = 0;
                break;
            }
            default:; break;
            }

            last_state = smoke_type;
        }
    }

} // namespace features::miscellaneous