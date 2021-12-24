#include <intrin.h>

#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/settings/settings.h"
#include "../engine/hash/hash.h"

c_demo_playback_paramaters *__fastcall hooks::get_demo_playback_parameters(uintptr_t ecx, uintptr_t edx) {

    // credits: https://github.com/danielkrupinski/Osiris/blob/1873b9b1cde1016da208815b4062dd07308d3b2f/Osiris/Hooks.cpp#L433

    const auto parameters = get_demo_playback_parameters_original(ecx, edx);

    if (parameters && settings.miscellaneous.reveal_overwatch_suspect &&
        (uintptr_t) _ReturnAddress() != patterns::get_demo_file_end_reached()) {
        static c_demo_playback_paramaters custom_parameters;
        custom_parameters = *parameters;
        custom_parameters.anonymous_player_identity = false;
        return &custom_parameters;
    }

    return get_demo_playback_parameters_original(ecx, edx);
}
