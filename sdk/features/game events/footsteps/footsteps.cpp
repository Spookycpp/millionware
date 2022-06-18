/*#include "footsteps.h"

#include <vector>

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"

#include "../../../engine/math/math.h"
#include "../../../engine/render/render.h"

namespace features::game_events::footsteps {
        std::vector< footstep_data_t > footsteps = {};

        void on_step_event(c_game_event *game_event) {

                if (!settings.visuals.player.footsteps)
                        return;

                const int  user_id = interfaces::engine_client->get_player_for_user_id(game_event->get_int(xs("userid")));
                const auto user = (c_player *)interfaces::entity_list->get_entity(user_id);

                if (!user || user == cheat::local_player || !user->is_valid() || user->get_team_num() ==
cheat::local_player->get_team_num()) return;

                footsteps.emplace_back(footstep_data_t{ user->get_abs_origin(), 255 });
        }

        void on_paint() { //@todo: make footsteps diappear after 1/2 a second or a second.
                if (!settings.visuals.player.footsteps || footsteps.empty())
                        return;

                for (size_t i = 0; i < footsteps.size(); ++i) {
                        auto &footstep = footsteps.at(i);

                        if (footstep.alpha <= 0) {
                                footsteps.erase(footsteps.begin() + i);
                                continue;
                        }

                        point_t screen;

                        if (!math::world_to_screen(footstep.position, screen))
                                continue;

                        render::draw_text({ screen.x, screen.y }, settings.visuals.player.footsteps_color, xs("Step"), FONT_TAHOMA_11);

                        footstep.alpha -= int(255.0f / 1.0f * interfaces::global_vars->frame_time);
                }
        }
}
*/