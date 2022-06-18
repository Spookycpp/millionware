#include <intrin.h>

#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/patterns/patterns.h"
#include "../core/settings/settings.h"

bool __fastcall hooks::is_connected(c_engine_client *ecx, uintptr_t edx) {

    if (!ecx || !interfaces::engine_client->is_in_game())
        return is_connected_original(ecx, edx);

    if (settings.miscellaneous.unlock_inventory && reinterpret_cast<uintptr_t>(_ReturnAddress()) == patterns::get_inventory_unlocker())
        return false;

    return is_connected_original(ecx, edx);
}
