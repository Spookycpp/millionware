#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/util/util.h"

#include "../core/cheat/cheat.h"
#include "../features/lagcompensation/lagcompensation.h"
#include "../features/miscellaneous/miscellaneous.h"
#include "../features/visuals/world/world.h"

void __fastcall hooks::frame_stage_notify(c_base_client_dll *ecx, uintptr_t edx, e_client_frame_stage frame_stage) {
    if (!interfaces::engine_client->is_in_game())
        return frame_stage_notify_original(ecx, edx, frame_stage);

    if (frame_stage == FRAME_STAGE_START)
        cheat::local_player = (c_player *) interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player());

    features::miscellaneous::on_frame_stage_notify(frame_stage);
    features::lag_compensation::on_frame_stage_notify(frame_stage);
    features::visuals::world::on_frame_stage_notify(frame_stage);

    util::on_frame_stage_notify(frame_stage);

    frame_stage_notify_original(ecx, edx, frame_stage);
}
