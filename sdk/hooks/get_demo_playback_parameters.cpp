#include <intrin.h>

#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../engine/hash/hash.h"

c_demo_playback_paramaters *__fastcall hooks::get_demo_playback_parameters(uintptr_t ecx, uintptr_t edx) {

    const auto parameters = get_demo_playback_parameters_original(ecx, edx);

    if (parameters /* config boolean here*/ && (uintptr_t)_ReturnAddress() != patterns::get_demo_file_end_reached()) {
        static c_demo_playback_paramaters custom_parameters;
        custom_parameters = *parameters;
        custom_parameters.anonymous_player_identity = false;
        return &custom_parameters;
    }

    return get_demo_playback_parameters_original(ecx, edx);
}
