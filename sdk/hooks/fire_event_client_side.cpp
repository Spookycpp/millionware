#include "../core/hooks/hooks.h"

bool _fastcall hooks::fire_event_client_side(uintptr_t ecx, uintptr_t edx, c_game_event *event) {

    if (!event)
        return fire_event_client_side_hk.call_original<decltype(&fire_event_client_side)>(ecx, edx, event);

    return fire_event_client_side_hk.call_original<decltype(&fire_event_client_side)>(ecx, edx, event);
}