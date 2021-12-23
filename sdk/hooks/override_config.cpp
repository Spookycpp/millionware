#include "../core/hooks/hooks.h"
#include "../core/settings/settings.h"

bool __fastcall hooks::override_config(uintptr_t ecx, uintptr_t edx, material_system_config_t *config, bool update) {

    if (settings.visuals.world.fullbright)
        config->full_bright = true;

    return override_config_hk.call_original<decltype(&override_config)>(ecx, edx, config, update);
}