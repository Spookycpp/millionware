#include "../../../engine/math/math.h"
#include "../../../engine/security/xorstr.h"
#include "../../../engine/render/render.h"
#include "../../../core/cheat/cheat.h"
#include "../../../core/util/util.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"
#include "../../../source engine/game_events.h"

#include "inferno.h"

#include <imgui.h>

namespace features::game_events::inferno {
    constexpr float inferno_duration = 7.03125f;

    std::vector<grenade_detonate_data_t> inferno_vec;

    void on_inferno_startburn(const grenade_detonate_data_t &data) {
        static c_convar *inferno_flame_lifetime = interfaces::convar_system->find_convar(xs("inferno_flame_lifetime"));

        inferno_vec.emplace_back(data);

        for (size_t i = 0; i < inferno_vec.size(); ++i) {
            grenade_detonate_data_t &inferno = inferno_vec.at(i);

            const float delta = interfaces::global_vars->current_time - inferno.start_time;

            if (std::abs(delta) > inferno_flame_lifetime->get_float()) {
                inferno_vec.erase(inferno_vec.begin() + i);
            }
        }
    }

    void draw() {
        if (inferno_vec.empty()) {
            return;
        }

        for (auto &inferno : inferno_vec) {
            c_entity *entity = interfaces::entity_list->get_entity(inferno.entity_id);

            if (!entity) {
                continue;
            }

            std::vector<point_t> convex_hull_points;
            std::vector<vector_t> points = get_points(entity);
            
            for (auto &point : points) {
                if (point_t screen_pos; math::world_to_screen(point, screen_pos)) {
                    convex_hull_points.emplace_back(screen_pos.x, screen_pos.y);
                }
            }

            jarvis_march jm{ convex_hull_points };
            std::vector<point_t> convex_hull = jm.compute_hull();

            render::fill_convex_poly(convex_hull.data(), convex_hull.size(), { 255, 0, 0, 75 });

            point_t screen_pos;
            if (!math::world_to_screen(entity->get_abs_origin(), screen_pos)) {
                continue;
            }

            const float seconds_left = inferno_duration - (interfaces::global_vars->current_time - inferno.start_time);

            // get damage to local player
            bool will_die = false;
            if (cheat::local_player && cheat::local_player->get_abs_origin().dist_2d(entity->get_abs_origin()) <= 300.0f) {
                float base_damage = 40.0f; // 40 damage per second

                base_damage *= seconds_left;

                const int hp_left = std::clamp(cheat::local_player->get_health() - static_cast<int>(base_damage), 0, 100);

                if (!hp_left) {
                    will_die = true;
                }
            }

            constexpr float min = math::deg_to_rad(270.0f);
            const float max = math::deg_to_rad(270.0f + 1.0f + 360.0f * (seconds_left / inferno_duration));
            if (min > max) {
                continue;
            }

            render::fill_circle(screen_pos, 12.0f, { 5, 5, 5, 155 });

            // draw progress
            const color_t color    = settings.visuals.world.grenades_color;
            const color_t bg_color = color_t::blend({ 33, 33, 33, 255 }, color, 0.3f);

            ImGui::GetOverlayDrawList()->PathArcTo({ screen_pos.x, screen_pos.y }, 10.0f, math::deg_to_rad(0.0f), math::deg_to_rad(360.0f));
            ImGui::GetOverlayDrawList()->PathStroke(IM_COL32(bg_color.r, bg_color.g, bg_color.b, 255), 0, 2);

            ImGui::GetOverlayDrawList()->PathArcTo({ screen_pos.x, screen_pos.y }, 10.0f, min, max);
            ImGui::GetOverlayDrawList()->PathStroke(IM_COL32(color.r, color.g, color.b, 255), 0, 2);

            // draw icon
            static IDirect3DTexture9 *molotov_tex = nullptr;
            static IDirect3DTexture9 *alert_tex = nullptr;
            if (!molotov_tex && !alert_tex) {
                molotov_tex = util::load_texture_from_vpk(xs("materials/panorama/images/icons/equipment/molotov.svg"));
                alert_tex = util::load_texture_from_vpk(xs("materials/panorama/images/icons/ui/alert.svg"));
            }

            if (molotov_tex && alert_tex) {
                render::draw_image({ screen_pos.x - 5.0f, screen_pos.y - 7.0f }, { 11.0f, 12.0f }, will_die ? color_t{ 230, 70, 70 } : color_t{ 255, 255, 255 }, will_die ? alert_tex : molotov_tex, 2.0f);
            }
        }
    }

    std::vector<vector_t> get_points(c_entity *inferno) {
        std::vector<vector_t> points;

        const vector_t abs_origin = inferno->get_abs_origin();

        for (int i = 0; i < inferno->get_fire_count(); ++i) {
            if (i > 100) { // MAX_INFERNO_FIRES
                continue;  // when would this ever happen
            }

            if (inferno->get_fire_is_burning()[i]) {
                vector_t point = {
                    static_cast<float>(inferno->get_fire_x_delta()[i]) + abs_origin.x,
                    static_cast<float>(inferno->get_fire_y_delta()[i]) + abs_origin.y,
                    static_cast<float>(inferno->get_fire_z_delta()[i]) + abs_origin.z
                };

                points.emplace_back(point);
            }
        }

        return points;
    }
}
