#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/settings/settings.h"
#include "../core/util/util.h"

#include "../engine/debug/debug_overlay.h"
#include "../engine/security/xorstr.h"

#include "../features/discord presence/rpc.h"
#include "../features/lagcompensation/lagcompensation.h"
#include "../features/miscellaneous/miscellaneous.h"
#include "../features/visuals/world/weather.h"
#include "../features/visuals/world/world.h"

void __fastcall hooks::frame_stage_notify(c_base_client_dll *ecx, uintptr_t edx, e_client_frame_stage frame_stage) {
    PROFILE_WITH(fsn[(int) frame_stage]);

    discord_rpc::update();
    features::miscellaneous::panorama_blur(); // main menu blur can be removed

    if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
        return frame_stage_notify_original(ecx, edx, frame_stage);

    if (frame_stage == e_client_frame_stage::FRAME_STAGE_RENDER_START) {
        static auto cl_csm_shadows = interfaces::convar_system->find_convar(xs("cl_csm_shadows"));

        if (settings.visuals.world.nightmode && cl_csm_shadows->get_int() == 1)
            cl_csm_shadows->set_value(0);
        else if (!settings.visuals.world.nightmode && cl_csm_shadows->get_int() == 0)
            cl_csm_shadows->set_value(1);

        // this will let glow & chams draw on agent models with patches.
        for (int i = 1; i <= interfaces::global_vars->max_clients; ++i) {
            auto player = (c_player *)interfaces::entity_list->get_entity(i);
            if (!player || player->get_life_state() != LIFE_STATE_ALIVE)
                continue;

            for (size_t n = 0; n < 5; n++) {
                // boris (c)
                if (player->player_patch_econ_indices()[n] != NULL)
                    player->player_patch_econ_indices()[n] = NULL;
            }
        }
    }

    if (frame_stage == e_client_frame_stage::FRAME_STAGE_START) {
        cheat::view_matrix = interfaces::engine_client->world_to_screen_matrix();
        cheat::local_player = static_cast<c_player *>(interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player()));
    }

    features::lag_compensation::on_frame_stage_notify(frame_stage);
    features::miscellaneous::on_frame_stage_notify(frame_stage);
    features::visuals::weather::on_frame_stage_notify(frame_stage);
    features::miscellaneous::skybox_changer(settings.visuals.world.skybox);

    frame_stage_notify_original(ecx, edx, frame_stage);
}
