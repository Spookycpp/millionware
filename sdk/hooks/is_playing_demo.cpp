#include <intrin.h>

#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/patterns/patterns.h"
#include "../core/settings/settings.h"

bool __fastcall hooks::is_playing_demo(c_engine_client *ecx, uintptr_t edx) {

    if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
        return is_playing_demo_original(ecx, edx);

    if (!settings.miscellaneous.money_reveal)
        return is_playing_demo_original(ecx, edx);

    const auto return_address = (uintptr_t) _ReturnAddress();
    const auto stack_frame = (uintptr_t) _AddressOfReturnAddress() - sizeof(uint32_t);

    if (return_address == patterns::get_is_demo_or_hltv() && *(uint32_t *) (stack_frame + 8) == patterns::get_money_reveal())
        return true;

    return is_playing_demo_original(ecx, edx);
}