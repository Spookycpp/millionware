#include <mutex>

#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/settings/settings.h"
#include "../features/visuals/entities/esp.h"

void __fastcall hooks::play_step_sound(c_player *ecx, uintptr_t edx, vector_t &origin, void *surface, float vol, bool force, void *arg) {
    if (settings.visuals.player.footsteps) {
        if (!ecx || !ecx->is_player() || ecx == cheat::local_player || ecx->get_team_num() == cheat::local_player->get_team_num()) {
            return play_step_sound_original(ecx, edx, origin, surface, vol, force, arg);
        }

        std::unique_lock lock(features::visuals::esp::footsteps_mutex);
        features::visuals::esp::footsteps.push_back({origin, interfaces::global_vars->current_time, 1.0f});
    }

    play_step_sound_original(ecx, edx, origin, surface, vol, force, arg);
}