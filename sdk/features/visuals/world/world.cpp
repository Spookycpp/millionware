#include "world.h"

#include <cstdio>
#include <string>
#include <windows.h>

#include <algorithm>
#include <cstdint>
#include <psapi.h>
#include <tlhelp32.h>
#include <unordered_map>

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"

#include "../../../engine/input/input.h"
#include "../../../engine/logging/logging.h"
#include "../../../engine/math/math.h"
#include "../../../engine/render/render.h"
#include "../../../engine/security/xorstr.h"

#include "../../../core/util/util.h"
#include "../../../source engine/entity.h"
#include <format>

namespace features::visuals::world {

    void on_frame_stage_notify(e_client_frame_stage frame_stage) {
        switch (frame_stage) {
        case e_client_frame_stage::FRAME_STAGE_RENDER_START: {
            nightmode();
        }
        }
    }

    void indicators() {
        if (!cheat::local_player || cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
            return;

        if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
            return;

        const auto screen_center = render::get_screen_size() * 0.5f;

        auto draw_indicator = [screen_center, offset = 400.0f](const char *text, const color_t &color) mutable {
            const auto measure = render::measure_text(text, FONT_VERDANA_24);

            render::draw_text(screen_center - measure * 0.5f + point_t(0.0f, offset), color, text, FONT_VERDANA_24);

            offset += measure.y + 8.0f;
        };

        if (settings.visuals.local.indicators & (1 << 0)) {
            static auto tick_prev = 0;
            static auto last_velocity = 0;
            static auto take_off = 0;
            static auto take_off_time = 0.0f;
            static auto last_on_ground = false;

            const auto vel = (int) (cheat::local_player->get_velocity().length_2d() + 0.5f);
            const auto on_ground = (cheat::local_player->get_flags() & ENTITY_FLAG_ONGROUND);

            if (last_on_ground && !on_ground) {
                take_off = vel;
                take_off_time = interfaces::global_vars->current_time + 1.0f;
            }

            last_on_ground = on_ground;

            const auto color = vel == last_velocity ? color_t(255, 199, 89) : vel < last_velocity ? color_t(255, 119, 119) : color_t(30, 255, 109);
            const auto should_draw_takeoff = (!on_ground || (take_off_time > interfaces::global_vars->current_time)) && settings.visuals.local.indicators & (1 << 1);

            char buffer[32];

            if (should_draw_takeoff)
                sprintf_s(buffer, xs("%i (%i)"), (int) vel, (int) take_off);
            else
                sprintf_s(buffer, xs("%i"), (int) vel);

            draw_indicator(buffer, color);

            if (tick_prev + 5 < interfaces::global_vars->tick_count) {
                last_velocity = vel;
                tick_prev = interfaces::global_vars->tick_count;
            }
        }

        if (settings.visuals.local.indicators & (1 << 2) && settings.miscellaneous.movement.jump_bug && input::is_key_down(settings.miscellaneous.movement.jump_bug_hotkey))
            draw_indicator(xs("jb"), {255, 255, 255, 220});

        if (settings.visuals.local.indicators & (1 << 4) && settings.miscellaneous.movement.edge_bug_assist && input::is_key_down(settings.miscellaneous.movement.edge_bug_assist_hotkey))
            draw_indicator(xs("eb"), {255, 255, 255, 220});

        if (settings.visuals.local.indicators & (1 << 5) && settings.miscellaneous.movement.edge_jump && input::is_key_down(settings.miscellaneous.movement.edge_jump_hotkey))
            draw_indicator(xs("ej"), {255, 255, 255, 220});
    }

    void nightmode() {
        static bool toggled = false;
        static float last_darkness = 0.f;

        auto do_nightmode = settings.visuals.world.nightmode != toggled || last_darkness != settings.visuals.world.nightmode_darkness || cheat::disconnect_state;

        if (!do_nightmode)
            return;

        toggled = settings.visuals.world.nightmode;
        last_darkness = settings.visuals.world.nightmode_darkness;

        if (!cheat::local_player || !interfaces::engine_client->is_in_game())
            return;

        cheat::disconnect_state = false;

        const static auto draw_specific_static_prop = interfaces::convar_system->find_convar(xs("r_DrawSpecificStaticProp"));

        const float world_darkness = settings.visuals.world.nightmode ? 1.f - (settings.visuals.world.nightmode_darkness / 100.f) : 1.f;
        const float prop_darkness = settings.visuals.world.nightmode ? 1.3f - (settings.visuals.world.nightmode_darkness / 100.f) : 1.f;

        draw_specific_static_prop->set_value(settings.visuals.world.nightmode ? 0 : -1);

        // iterate material handles.
        for (auto handle = interfaces::material_system->first_material(); handle != interfaces::material_system->invalid_material(); handle = interfaces::material_system->next_material(handle)) {

            // get material from handle.
            c_material *material = interfaces::material_system->get_material(handle);
            if (!material)
                continue;

            if (material->is_error_material())
            continue;

            // modulate world materials.
            if (strncmp(material->get_group_name(), xs("World textures"), 14) == 0)
                material->set_color(world_darkness, world_darkness, world_darkness);
            // modulate props materials.
            else if (strncmp(material->get_group_name(), xs("StaticProp textures"), 19) == 0)
                material->set_color(prop_darkness, prop_darkness, prop_darkness);

            auto cl_csm_shadows = interfaces::convar_system->find_convar(xs("cl_csm_shadows"));
            if (cl_csm_shadows->get_int() != 0)
                cl_csm_shadows->set_value(0);
        }
    }

    void display_spectators() {
        if (!settings.visuals.local.spectator_list)
            return;

        if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
            return;

        const auto obs_mode_to_string = [](int obs_mode) -> std::string {
            switch (obs_mode) {
            // clang-format off
                case OBS_MODE_IN_EYE:    return xs("firstperson");
                case OBS_MODE_CHASE:     return xs("thirdperson");
                default:                 return "";
                // clang-format on
            }
        };

        for (auto i = 1, y = 0; i <= 64; i++) {

            const auto ent = (c_player *) interfaces::entity_list->get_entity(i);

            if (!cheat::local_player)
                continue;

            if (!ent || !ent->is_player() || ent->get_networkable()->is_dormant())
                continue;

            if (ent->get_life_state() == LIFE_STATE_ALIVE)
                continue;

            if (ent->get_team_num() != cheat::local_player->get_team_num())
                continue;

            const auto obs_target = ent->get_observer_target().get();
            const auto obs_mode = ent->get_observer_mode();

            player_info_t info;

            if (!interfaces::engine_client->get_player_info(i, info))
                continue;

            if (obs_target) {
                player_info_t target_info;

                if (!interfaces::engine_client->get_player_info(obs_target->get_networkable()->index(), target_info))
                    continue;

                const auto obs_mode_str = obs_mode_to_string(obs_mode);

                if (obs_mode_str.empty())
                    continue;

                const auto string = std::format(xs("{} -> {} ({})"), info.name, target_info.name, obs_mode_str);
                const auto screen_size = render::get_screen_size();
                const auto text_size = render::measure_text(string.c_str(), FONT_TAHOMA_11);

                render::draw_text({screen_size.x - text_size.x - 4.0f, 4.0f + y++ * 16.0f}, {255, 255, 255, 255}, string.c_str(), FONT_TAHOMA_11);
            }
        }
    }

    void draw_world(c_entity *entity) {
        c_client_class *client_class = entity->get_networkable()->get_client_class();

        if (!client_class)
            return;

        if (client_class->class_id == CPlantedC4)
            bomb_timer(entity);
    }

    void bomb_timer(c_entity *entity) {

        const auto get_bombsite = [&]() -> std::string {
            const auto player_res = util::get_player_resource();

            if (player_res == nullptr)
                return xs("<unknown>");

            const auto bomb_origin = entity->get_abs_origin();
            const auto &site_a = player_res->get_bomb_site_center_a();
            const auto &site_b = player_res->get_bomb_site_center_b();

            const auto dist_to_site_a = bomb_origin.dist(site_a);
            const auto dist_to_site_b = bomb_origin.dist(site_b);

            if (dist_to_site_a < dist_to_site_b)
                return xs("A");

            return xs("B");
        };

        const auto get_bomb_time = [&]() -> std::string {
            if (!entity->get_is_bomb_ticking() || entity->get_is_bomb_defused())
                return {};

            const float bomb_time = (interfaces::global_vars->current_time - entity->get_c4_blow()) * -1.0f;

            return std::format(xs("{}: {:.2f}s"), get_bombsite(), bomb_time);
        };

        const auto get_bomb_damage = [&]() {
            const vector_t delta = entity->get_renderable()->get_render_origin() - cheat::local_player->get_renderable()->get_render_origin();

            const float bomb_damage = 500.0f * std::exp(-(delta.length() * delta.length() / (1750.0f * 0.33333334f * 2.0f * (1750.0f * 0.33333334f))));
            float damage = bomb_damage;

            const int armor_value = cheat::local_player->get_armor();

            if (armor_value > 0) {
                damage *= 0.5f;

                if ((bomb_damage - damage) * 0.5f > float(armor_value))
                    damage = bomb_damage - float(armor_value) * (1.0f / 0.5f);
            }

            return cheat::local_player->get_health() - int(std::round(damage));
        };

        if (!settings.visuals.world.bomb)
            return;

        const std::string bomb_time = get_bomb_time();

        if (bomb_time.empty())
            return;

        const auto bomb_time_str = get_bomb_time();
        const auto health_remaining = get_bomb_damage();
        const auto health_remaining_text = std::format(xs("Health remaining: {} HP"), std::max(health_remaining, 0));

        const auto screen_size = render::get_screen_size();
        const auto bomb_time_text_size = render::measure_text(bomb_time_str.c_str(), FONT_TAHOMA_11);
        const auto health_remaining_text_size = render::measure_text(health_remaining_text.c_str(), FONT_TAHOMA_11);

        render::draw_text({screen_size.x * 0.5f - bomb_time_text_size.x * 0.5f, screen_size.y * 0.15f}, {255, 255, 255, 255}, bomb_time_str.c_str(), FONT_TAHOMA_11);

        if (cheat::local_player->get_life_state() == LIFE_STATE_ALIVE)
            render::draw_text({screen_size.x * 0.5f - health_remaining_text_size.x * 0.5f, screen_size.y * 0.15f + bomb_time_text_size.y + 5.f},
                              health_remaining > 0 ? color_t{33, 255, 33, 235} : color_t{220, 33, 33, 235}, health_remaining_text.c_str(), FONT_TAHOMA_11);
    }

} // namespace features::visuals::world