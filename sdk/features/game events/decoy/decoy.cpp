#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"
#include "../../../core/util/util.h"
#include "../../../engine/math/math.h"
#include "../../../engine/render/render.h"
#include "../../../engine/security/xorstr.h"
#include "../../../source engine/game_events.h"

#include "decoy.h"

#include <imgui.h>

namespace features::game_events::decoy {
    constexpr float decoy_duration = 15.0f;

    std::vector<grenade_detonate_data_t> decoy_vec;

    void on_decoy_started(const grenade_detonate_data_t &data) {
        if (!settings.visuals.world.grenades) {
            return;
        }

        decoy_vec.emplace_back(data);
    }

    void reset() {
        decoy_vec.clear();
    }

    void draw() {
        if (!settings.visuals.world.grenades) {
            return;
        }

        if (decoy_vec.empty()) {
            return;
        }

        for (size_t i = 0; i < decoy_vec.size(); ++i) {
            grenade_detonate_data_t &decoy = decoy_vec.at(i);

            const float delta = interfaces::global_vars->current_time - decoy.start_time;

            if (std::abs(delta) > decoy_duration) {
                decoy_vec.erase(decoy_vec.begin() + i);
            }
        }

        for (auto &decoy : decoy_vec) {
            c_entity *entity = interfaces::entity_list->get_entity(decoy.entity_id);

            if (!entity) {
                continue;
            }

            point_t screen_pos;
            if (!math::world_to_screen(entity->get_abs_origin(), screen_pos)) {
                continue;
            }

            const float seconds_left = decoy_duration - (interfaces::global_vars->current_time - decoy.start_time);

            constexpr float min = math::deg_to_rad(270.0f);
            const float max = math::deg_to_rad(270.0f + 1.0f + 360.0f * (seconds_left / decoy_duration));
            if (min > max) {
                continue;
            }

            render::fill_circle(screen_pos, 12.0f, {5, 5, 5, 155});

            // draw progress
            const color_t color = settings.visuals.world.grenades_color;
            const color_t bg_color = color_t::blend({33, 33, 33, 255}, color, 0.3f);

            ImGui::GetForegroundDrawList()->PathArcTo({screen_pos.x, screen_pos.y}, 13.0f, min, max);
            ImGui::GetForegroundDrawList()->PathStroke(IM_COL32(bg_color.r, bg_color.g, bg_color.b, 65), 0, 1);

            ImGui::GetForegroundDrawList()->PathArcTo({screen_pos.x, screen_pos.y}, 12.0f, math::deg_to_rad(0.0f),
                                                      math::deg_to_rad(360.0f));
            ImGui::GetForegroundDrawList()->PathStroke(IM_COL32(bg_color.r, bg_color.g, bg_color.b, 185), 0, 2);

            ImGui::GetForegroundDrawList()->PathArcTo({screen_pos.x, screen_pos.y}, 10.0f, min, max);
            ImGui::GetForegroundDrawList()->PathStroke(IM_COL32(color.r, color.g, color.b, 255), 0, 2);

            // draw icon
            static IDirect3DTexture9 *decoy_tex = nullptr;
            if (!decoy_tex) {
                decoy_tex = util::load_texture_from_vpk(xs("materials/panorama/images/icons/equipment/decoy.svg"));
            }

            if (decoy_tex) {
                render::draw_image({screen_pos.x - 5.0f, screen_pos.y - 7.0f}, {11.0f, 12.0f}, color_t{255, 255, 255}, decoy_tex, 2.0f);
            }
        }
    }
} // namespace features::game_events::decoy
