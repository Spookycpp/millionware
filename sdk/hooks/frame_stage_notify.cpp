#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/util/util.h"

#include "../core/cheat/cheat.h"
#include "../features/lagcompensation/lagcompensation.h"
#include "../features/miscellaneous/miscellaneous.h"
#include "../features/visuals/world/weather.h"
#include "../features/visuals/world/world.h"
#include "../core/settings/settings.h"

void __fastcall hooks::frame_stage_notify(c_base_client_dll *ecx, uintptr_t edx, e_client_frame_stage frame_stage) {
    if (!interfaces::engine_client->is_in_game())
        return frame_stage_notify_original(ecx, edx, frame_stage);

    if (frame_stage == e_client_frame_stage::FRAME_STAGE_START) {
        cheat::local_player = (c_player *) interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player());
    
        if (cheat::local_player) {
            static bool should_call = false;

            if (should_call && !settings.visuals.world.weather) {
                features::visuals::weather::reset_weather();
                should_call = false;
            }

            if (!should_call && settings.visuals.world.weather) {
                cheat::created_rain = false;
                should_call = true;
            }

            if (should_call) {
                features::visuals::weather::update_weather();
            }
        }
    
    }

    features::miscellaneous::on_frame_stage_notify(frame_stage);
    features::lag_compensation::on_frame_stage_notify(frame_stage);
    features::visuals::world::on_frame_stage_notify(frame_stage);

    util::on_frame_stage_notify(frame_stage);

    frame_stage_notify_original(ecx, edx, frame_stage);
}
