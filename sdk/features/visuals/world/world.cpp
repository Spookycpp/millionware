#include "world.h"

#include <cstdio>
#include <format>
#include <string>
#include <windows.h>

#include <algorithm>
#include <cstdint>
#include <format>
#include <psapi.h>
#include <tlhelp32.h>
#include <unordered_map>

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"
#include "../../../core/util/util.h"

#include "../../../engine/input/input.h"
#include "../../../engine/logging/logging.h"
#include "../../../engine/math/math.h"
#include "../../../engine/render/render.h"
#include "../../../engine/security/xorstr.h"
#include "../../../source engine/entity.h"
#include "../../../ui/ui.h"

namespace features::visuals::world {
    void indicators() {
        if (!cheat::local_player || cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
            return;

        if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
            return;

        const point_t screen_size = render::get_screen_size();

        auto draw_indicator = [screen_size, offset = 8.f](const char *text, const color_t &color) mutable {
            static c_convar *safezoney = interfaces::convar_system->find_convar(xs("safezoney"));
            static c_convar *hud_scaling = interfaces::convar_system->find_convar(xs("hud_scaling"));

            float y_pos = 900.f * hud_scaling->get_float() + (screen_size.y - screen_size.y * safezoney->get_float() + 1.f) / 2.f;
            const point_t indicator_text_size = render::measure_text(text, FONT_VERDANA_24);

            render::draw_text_outlined({screen_size.x / 2.0f - indicator_text_size.x / 2.0f, y_pos - 15.0f + (0.f + offset)}, color,
                                       {5, 5, 5, 220}, text, FONT_VERDANA_24);

            offset += indicator_text_size.y + 8.f;
        };

        if (settings.visuals.local.indicators & (1 << 0)) {
            static auto vel = 0;
            static bool on_ground = false;
            static auto tick_prev = 0;
            static auto last_velocity = 0;
            static auto take_off = 0;
            static auto take_off_time = 0.0f;
            static auto last_on_ground = false;

            vel = (int) (cheat::local_player->get_velocity().length_2d() + 0.5f);
            on_ground = (cheat::local_player->get_flags() & ENTITY_FLAG_ONGROUND);

            if (last_on_ground && !on_ground) {
                take_off = vel;
                take_off_time = interfaces::global_vars->current_time + 1.0f;
            }

            last_on_ground = on_ground;

            const auto color = vel == last_velocity  ? settings.visuals.local.velocity_color_1
                               : vel < last_velocity ? settings.visuals.local.velocity_color_2
                                                     : settings.visuals.local.velocity_color_3;
            const auto should_draw_takeoff =
                (!on_ground || (take_off_time > interfaces::global_vars->current_time)) && settings.visuals.local.indicators & (1 << 1);

            char buffer[32];

            if (should_draw_takeoff)
                sprintf_s(buffer, xs("%i (%i)"), (int) vel, (int) take_off);
            else
                sprintf_s(buffer, xs("%i"), (int) vel);

            // don't look at me like that.
            if (!cheat::local_player->is_sane()) {
                vel = 0;
                on_ground = false;
                tick_prev = 0;
                last_velocity = 0;
                take_off = 0;
                take_off_time = 0.0f;
                last_on_ground = false;
            }

            draw_indicator(buffer, color);

            if (tick_prev + 5 < interfaces::global_vars->tick_count) {
                last_velocity = vel;
                tick_prev = interfaces::global_vars->tick_count;
            }
        }

        if (settings.visuals.local.indicators & (1 << 2) && settings.miscellaneous.movement.jump_bug &&
            input::is_key_down(settings.miscellaneous.movement.jump_bug_hotkey))
            draw_indicator(xs("jb"), {255, 255, 255, 220});

        if (settings.visuals.local.indicators & (1 << 4) && settings.miscellaneous.movement.edge_bug_assist &&
            input::is_key_down(settings.miscellaneous.movement.edge_bug_assist_hotkey))
            draw_indicator(xs("eb"), {255, 255, 255, 220});

        if (settings.visuals.local.indicators & (1 << 5) && settings.miscellaneous.movement.edge_jump &&
            input::is_key_down(settings.miscellaneous.movement.edge_jump_hotkey))
            draw_indicator(xs("ej"), {255, 255, 255, 220});

        if (settings.visuals.local.indicators & (1 << 6) && settings.miscellaneous.movement.long_jump &&
            input::is_key_down(settings.miscellaneous.movement.long_jump_hotkey))
            draw_indicator(xs("lj"), {255, 255, 255, 220});
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

            const auto obs_target = ent->get_observer_target().get();
            const auto obs_mode = ent->get_observer_mode();

            player_info_t info;

            if (!interfaces::engine_client->get_player_info(i, info))
                continue;

            if (!strcmp(info.name, xs("GOTV")))
                continue;

            if (obs_target) {

                if (cheat::local_player->get_life_state() == LIFE_STATE_ALIVE) {
                    if (obs_target != cheat::local_player)
                        continue;
                } else {
                    if (obs_target != cheat::local_player->get_observer_target().get())
                        continue;

                    if (obs_target == cheat::local_player)
                        continue;
                }

                player_info_t target_info;

                if (!interfaces::engine_client->get_player_info(obs_target->get_networkable()->index(), target_info))
                    continue;

                const auto obs_mode_str = obs_mode_to_string(obs_mode);

                if (obs_mode_str.empty())
                    continue;

                const auto string = std::format(xs("{}"), info.name);
                const auto screen_size = render::get_screen_size();
                const auto text_size = render::measure_text(string.c_str(), FONT_TAHOMA_11);

                render::draw_text({screen_size.x - text_size.x - 4.0f, 4.0f + y++ * 16.0f}, {255, 255, 255, 255}, string.c_str(),
                                  FONT_TAHOMA_11);
            }
        }
    }

    void draw_world(c_entity *entity) {
        c_client_class *client_class = entity->get_networkable()->get_client_class();

        if (!client_class)
            return;

        if (client_class->class_id == CPlantedC4) {
            bomb_timer(entity);
        }
    }

    void bomb_timer(c_entity *entity) {
        if (!settings.visuals.world.planted_bomb) {
            return;
        }

        if (!entity->get_is_bomb_ticking() && cheat::round_changed == true) {
            return;
        }

        const c_player_resource *player_res = util::get_player_resource();
        if (!player_res) {
            return;
        }

        const vector_t bomb_origin = entity->get_abs_origin();

        const auto get_bomb_damage = [&]() {
            vector_t src = bomb_origin;
            src.z += 1.0f;

            constexpr float bomb_radius = 500.0f; // TODO: radius should be retrieved from the BSP
            constexpr float sigma = bomb_radius * 3.5f / 3.0f;

            const float dist = (cheat::local_player->get_renderable()->get_render_origin() - src).length();
            const float damage = bomb_radius * std::exp(-dist * dist / (2.0f * sigma * sigma));

            float scaled_damage = damage;

            // TODO: take into account heavy armor
            if (const int armor_value = cheat::local_player->get_armor(); armor_value > 0) {
                float new_damage = damage * 0.5f;
                float armor = (damage - new_damage) * 0.5f;

                if (armor > static_cast<float>(armor_value)) {
                    armor = static_cast<float>(armor_value) * 2.0f;
                    new_damage = damage - armor;
                }

                scaled_damage = std::max(0.0f, std::floor(new_damage));
            }

            return static_cast<int>(scaled_damage);
        };

        // determine which site the bomb is planted at
        std::string site;

        if (bomb_origin.dist(player_res->get_bomb_site_center_a()) < bomb_origin.dist(player_res->get_bomb_site_center_b())) {
            site = xs("A");
        } else {
            site = xs("B");
        }

        // get bomb time left
        float time_left = std::max(entity->get_bomb_blow_time() - interfaces::global_vars->current_time, 0.0f);

        if (time_left <= 0.0f) {
            return;
        }

        const int bomb_damage = get_bomb_damage();
        const int health_left = cheat::local_player->get_health() - bomb_damage;

        // draw
        static c_convar *mp_c4timer = interfaces::convar_system->find_convar(xs("mp_c4timer"));
        static c_convar *safezoney = interfaces::convar_system->find_convar(xs("safezoney"));
        static c_convar *hud_scaling = interfaces::convar_system->find_convar(xs("hud_scaling"));

        const point_t screen_size = render::get_screen_size();

        float y_pos = 95.0f * hud_scaling->get_float() + (screen_size.y - screen_size.y * safezoney->get_float() + 1.0f) / 2.0f;

        // timer bar
        constexpr float bar_width = 250.0f;
        constexpr float bar_height = 6.0f;

        render::fill_rect({screen_size.x / 2.0f - bar_width / 2.0f - 1.0f, y_pos - 1.0f}, {bar_width + 2.0f, bar_height + 2.0f},
                          {34, 34, 34, 170});
        render::fill_rect({screen_size.x / 2.0f - bar_width / 2.0f, y_pos}, {time_left * bar_width / mp_c4timer->get_float(), bar_height},
                          ui::get_accent_color());

        const auto defusing_player = reinterpret_cast<c_player *>(entity->get_bomb_defuser().get());
        if (defusing_player) {
            std::string defuse_text = xs("Defusing");

            const float defuse_time = std::max(entity->get_defuse_countdown() - interfaces::global_vars->current_time, 0.0f);
            if (defuse_time <= time_left) {
                time_left = defuse_time;
            } else {
                defuse_text += xs(" (no time left)");
            }

            const float max_defuse_time = defusing_player->get_has_defuser() ? 5.0f : 10.0f;

            const point_t defuse_text_size = render::measure_text(defuse_text.c_str(), FONT_CEREBRI_SANS_BOLD_13);
            render::draw_text_outlined({screen_size.x / 2.0f - defuse_text_size.x / 2.0f, y_pos - 15.0f}, {255, 255, 255}, {5, 5, 5, 220},
                                       defuse_text.c_str(), FONT_CEREBRI_SANS_BOLD_13);

            y_pos += bar_height + 4.0f;

            render::fill_rect({screen_size.x / 2.0f - bar_width / 2.0f - 1.0f, y_pos - 1.0f}, {bar_width + 2.0f, bar_height + 2.0f},
                              {34, 34, 34, 170});
            render::fill_rect({screen_size.x / 2.0f - bar_width / 2.0f, y_pos}, {time_left * bar_width / max_defuse_time, bar_height},
                              {119, 164, 237});
        }

        // text
        const std::string time_remaining_text = std::format(xs("{}: {:.2f}s"), site, time_left);
        const std::string health_remaining_text = std::format(xs("{} HP remaining"), std::max(health_left, 0));
        const point_t c4_time_left_size = render::measure_text(time_remaining_text.c_str(), FONT_CEREBRI_SANS_BOLD_13);
        const point_t hp_left_size = render::measure_text(health_remaining_text.c_str(), FONT_CEREBRI_SANS_BOLD_13);

        render::gradient_h({screen_size.x / 2.0f - 80.0f - 4.0f, y_pos + bar_height + 3.0f},
                           {80.0f, hp_left_size.y + c4_time_left_size.y + 0.0f}, {5, 5, 5, 0}, {5, 5, 5, 90});
        render::gradient_h({screen_size.x / 2.0f - 4.0f, y_pos + bar_height + 3.0f},
                           {80.0f + 4.0f, hp_left_size.y + c4_time_left_size.y + 0.0f}, {5, 5, 5, 90}, {5, 5, 5, 0});

        render::draw_text_outlined({screen_size.x / 2.0f - c4_time_left_size.x / 2.0f, y_pos + bar_height + 4.0f}, {255, 255, 255},
                                   {5, 5, 5, 220}, time_remaining_text.c_str(), FONT_CEREBRI_SANS_BOLD_13);

        const color_t hp_remaining_color =
            color_t::blend({0, 255, 0}, {255, 0, 0},
                           std::clamp(static_cast<float>(bomb_damage) / static_cast<float>(cheat::local_player->get_health()), 0.0f, 1.0f));

        if (cheat::local_player->get_life_state() == LIFE_STATE_ALIVE) {
            // health remaining
            render::draw_text_outlined({screen_size.x / 2.0f - hp_left_size.x / 2.0f, y_pos + bar_height + 3.0f + c4_time_left_size.y},
                                       hp_remaining_color, {5, 5, 5, 220}, health_remaining_text.c_str(), FONT_CEREBRI_SANS_BOLD_13);
        }
    }
} // namespace features::visuals::world