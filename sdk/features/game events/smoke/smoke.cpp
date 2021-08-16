#include "../../../engine/math/math.h"
#include "../../../engine/security/xorstr.h"
#include "../../../engine/render/render.h"
#include "../../../core/cheat/cheat.h"
#include "../../../core/util/util.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"
#include "../../../source engine/game_events.h"

#include "smoke.h"

#include <imgui.h>

namespace features::game_events::smoke {
    constexpr float smoke_duration = 15.0f;

    std::vector<grenade_detonate_data_t> smoke_vec;

	void on_smokegrenade_detonate(const grenade_detonate_data_t &data) {
        smoke_vec.emplace_back(data);

        for (size_t i = 0; i < smoke_vec.size(); ++i) {
            grenade_detonate_data_t &smoke = smoke_vec.at(i);

            const float delta = interfaces::global_vars->current_time - smoke.start_time;

            if (std::abs(delta) > smoke_duration) {
                smoke_vec.erase(smoke_vec.begin() + i);
            }
        }
	}

    void draw() {
        if (smoke_vec.empty()) {
            return;
        }

        for (auto &smoke : smoke_vec) {
            c_entity *entity = interfaces::entity_list->get_entity(smoke.entity_id);

            if (!entity) {
                continue;
            }

            point_t screen_pos;
            if (!math::world_to_screen(entity->get_abs_origin(), screen_pos)) {
                continue;
            }

            const float seconds_left = smoke_duration - (interfaces::global_vars->current_time - smoke.start_time);

            constexpr float min = math::deg_to_rad(270.0f);
            const float max = math::deg_to_rad(270.0f + 1.0f + 360.0f * (seconds_left / smoke_duration));
            if (min > max) {
                continue;
            }

            render::fill_circle(screen_pos, 12.0f, { 5, 5, 5, 155 });

            // draw progress
            const color_t color = settings.visuals.world.grenades_color;
            const color_t bg_color = color_t::blend({ 33, 33, 33, 255 }, color, 0.3f);

            ImGui::GetOverlayDrawList()->PathArcTo({ screen_pos.x, screen_pos.y }, 10.0f, math::deg_to_rad(0.0f), math::deg_to_rad(360.0f));
            ImGui::GetOverlayDrawList()->PathStroke(IM_COL32(bg_color.r, bg_color.g, bg_color.b, 255), 0, 2);

            ImGui::GetOverlayDrawList()->PathArcTo({ screen_pos.x, screen_pos.y }, 10.0f, min, max);
            ImGui::GetOverlayDrawList()->PathStroke(IM_COL32(color.r, color.g, color.b, 255), 0, 2);

            // draw icon
            static IDirect3DTexture9 *smoke_tex = nullptr;
            if (!smoke_tex) {
                smoke_tex = util::load_texture_from_vpk(xs("materials/panorama/images/icons/equipment/smokegrenade.svg"));
            }

            if (smoke_tex) {
                render::draw_image({ screen_pos.x - 2.0f, screen_pos.y - 7.0f }, { 5.0f, 12.0f }, color_t{ 255, 255, 255 }, smoke_tex, 2.0f);
            }
        }
    }
}
