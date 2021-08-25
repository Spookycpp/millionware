/*
#include <mutex>

#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/settings/settings.h"
#include "../features/visuals/entities/esp.h"

void __fastcall hooks::play_footstep_sound(uintptr_t ecx, uintptr_t edx, const vector_t &origin, bool left_foot, bool feet_in_water, bool knees_in_water, bool jumping) {
    std::unique_lock lock(features::visuals::esp::footsteps_mutex);
    features::visuals::esp::footsteps.push_back({ origin, interfaces::global_vars->current_time, 1.0f });
    play_footstep_sound_original(ecx, edx, origin, left_foot, feet_in_water, knees_in_water, jumping);
}
*/