#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/settings/settings.h"

#include "../source engine/glow.h"

int __fastcall hooks::do_post_screen_effects(c_client_mode *ecx, uintptr_t edx, int effect_id) {

    static void *glow_manager = nullptr;

    if (!glow_manager)
        glow_manager = *reinterpret_cast<void **>((patterns::get_glow_manager() + 3));

    if (!glow_manager)
        return do_post_screen_effects_original(ecx, edx, effect_id);

    // sanity checks.
    if (!cheat::local_player)
        return do_post_screen_effects_original(ecx, edx, effect_id);

    if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
        return do_post_screen_effects_original(ecx, edx, effect_id);

    if (!settings.visuals.player.glow)
        return do_post_screen_effects_original(ecx, edx, effect_id);

    glow_object_definition_t *glow_objects = *(glow_object_definition_t **) glow_manager;
    int glow_size = *(int *) ((uint32_t) glow_manager + 0x4);

    for (int i = 1; i <= 64; i++) {

        auto entity = (c_player *) interfaces::entity_list->get_entity(i);

        if (!entity || entity->get_team_num() == cheat::local_player->get_team_num())
            continue;

        auto index = entity->get_glow_index();

        if (index < glow_size) {
            glow_objects[index].set(
                settings.visuals.player.glow_color.r / 255.f,
                settings.visuals.player.glow_color.g / 255.f,
                settings.visuals.player.glow_color.b / 255.f,
                settings.visuals.player.glow_color.a / 255.f);
        }
    }

    return do_post_screen_effects_original(ecx, edx, effect_id);
}
