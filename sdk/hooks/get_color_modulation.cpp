#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/settings/settings.h"

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/374249-somewhat-simpler-doing-night-mode.html
void __fastcall hooks::get_color_modulation(uintptr_t ecx, uintptr_t edx, float &r, float &g, float &b) {

    get_color_modulation_original(ecx, edx, r, g, b);

    if (!settings.visuals.world.nightmode)
        return;

    const auto material = reinterpret_cast<c_material *>(ecx);

    if (!material || material->is_error_material())
        return;

    auto group = material->get_group_name();

    // exclude stuff we dont want modulated
    if (strcmp(group, xs("World textures")) && strcmp(group, xs("StaticProp textures")) && strcmp(group, xs("SkyBox textures")))
        return;

    bool is_prop = strcmp(group, xs("StaticProp textures")) == 0;

    r *= 1.0f - (is_prop ? 0.6f : 0.93f) * (settings.visuals.world.nightmode_darkness / 100.0f);
    g *= 1.0f - (is_prop ? 0.6f : 0.93f) * (settings.visuals.world.nightmode_darkness / 100.0f);
    b *= 1.0f - (is_prop ? 0.6f : 0.93f) * (settings.visuals.world.nightmode_darkness / 100.0f);
}