#include "../core/cheat/cheat.h"
#include "../core/interfaces/interfaces.h"
#include "../core/hooks/hooks.h"
#include "../core/settings/settings.h"

#include "../engine/security/xorstr.h"

#include "../source engine/entity.h"
#include "../source engine/material.h"
#include "../source engine/material_handle.h"
#include "../engine/logging/logging.h"

void __fastcall hooks::draw_model_execute(uintptr_t ecx, uintptr_t edx, void* ctx, void* state, c_model_render_info* info, matrix3x4_t* matrix) {

    static c_material* textured = nullptr;
    static c_material* flat = nullptr;
    static c_material* glow = nullptr;

    if (!textured || !flat) {
        textured = interfaces::material_system->find_material(XORSTR("debug/debugambientcube"));
        flat = interfaces::material_system->find_material(XORSTR("debug/debugdrawflat"));
        glow = interfaces::material_system->find_material(XORSTR("dev/glow_armsrace"));

        // fixing crashes related to map change
        textured->increment_reference_count();
        flat->increment_reference_count();
        glow->increment_reference_count();
    }

    if (info->flags != 1 || strstr(info->model->name, XORSTR("models/player")) == nullptr) {
        draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
        return;
    }

    c_player* entity = (c_player*)interfaces::entity_list->get_entity(info->entity_index);

    const auto& visible_color = settings.visuals.player.chams.visible_color;
    const auto& invisible_color = settings.visuals.player.chams.invisible_color;

    if (entity == nullptr || !entity->is_enemy()) {
        interfaces::model_render->force_material_override(nullptr);

        draw_model_execute_original(ecx, edx, ctx, state, info, matrix);

        return;
    }

    c_material* material = nullptr;

    switch (settings.visuals.player.chams.material) {
        case 0: material = textured; break;
        case 1: material = flat;	 break;
    }

    if (settings.visuals.player.chams.invisible) {
        material->set_color(invisible_color);
        material->set_flag(MATERIAL_FLAG_IGNORE_Z, true);

        interfaces::model_render->force_material_override(material);

        draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
    }

    if (settings.visuals.player.chams.visible) {
        material->set_color(visible_color);
        material->set_flag(MATERIAL_FLAG_IGNORE_Z, false);

        interfaces::model_render->force_material_override(material);

        draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
    }
    else {
        interfaces::model_render->force_material_override(nullptr);

        draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
    }

    draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
}