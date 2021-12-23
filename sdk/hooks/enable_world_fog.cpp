#include "../core/hooks/hooks.h"
#include "../core/settings/settings.h"

void __fastcall hooks::enable_world_fog() {
    if (settings.visuals.world.remove_fog)
        return;

    return enable_world_fog_hk.call_original<decltype(&enable_world_fog)>();
}