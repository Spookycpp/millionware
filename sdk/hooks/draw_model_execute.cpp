#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/settings/settings.h"

#include "../engine/debug/debug_overlay.h"
#include "../engine/security/xorstr.h"

#include "../core/util/util.h"
#include "../engine/logging/logging.h"
#include "../source engine/entity.h"
#include "../source engine/material.h"
#include "../source engine/material_handle.h"


void __fastcall hooks::draw_model_execute(uintptr_t ecx, uintptr_t edx, void *ctx, void *state, c_model_render_info *info,
                                          matrix3x4_t *matrix) {

    PROFILE_WITH(dme);

    static c_material *textured = nullptr;
    static c_material *flat = nullptr;
    static c_material *glow = nullptr;

    if (!textured || !flat || !glow) {
        textured = interfaces::material_system->find_material(xs("debug/debugambientcube"));
        flat = interfaces::material_system->find_material(xs("debug/debugdrawflat"));
        glow = interfaces::material_system->find_material(xs("dev/glow_armsrace"));

        // fixing crashes related to map change
        textured->increment_reference_count();
        flat->increment_reference_count();
        glow->increment_reference_count();

        util::disable_model_occlusion();
    }

    const auto weapon_color = settings.visuals.local.chams.weapon_color;
    const auto arm_color = settings.visuals.local.chams.arms_color;
    const auto sleeve_color = settings.visuals.local.chams.sleeve_color;

    const auto weapon_material = settings.visuals.local.chams.weapon_material == 0 ? textured : flat;
    const auto arm_material = settings.visuals.local.chams.arms_material == 0 ? textured : flat;
    const auto sleeve_material = settings.visuals.local.chams.sleeve_material == 0 ? textured : flat;

<<<<<<< HEAD
    const auto is_weapon = settings.visuals.local.chams.weapon && !strstr(info->model->name, xs("sleeve")) && strstr(info->model->name, xs("models/weapons/v_")) && !strstr(info->model->name, xs("arms"));
    const auto is_arms = settings.visuals.local.chams.arms && strstr(info->model->name, xs("arm")) && !strstr(info->model->name, xs("v_sleeve"));
    const auto is_sleeve = settings.visuals.local.chams.sleeve && strstr(info->model->name, xs("v_sleeve"));

    if (is_weapon) {
        weapon_material->set_color(weapon_color);
        weapon_material->set_alpha(weapon_color.a);
        weapon_material->set_flag(MATERIAL_FLAG_IGNORE_Z, false);

        interfaces::model_render->force_material_override(weapon_material);
        draw_model_execute_hk.call_original<decltype(&draw_model_execute)>(ecx, edx, ctx, state, info, matrix);
        interfaces::model_render->force_material_override(nullptr);
        return;
=======
    if (settings.visuals.local.chams.weapon) {
        if (!strstr(info->model->name, xs("sleeve")),
            strstr(info->model->name, xs("models/weapons/v_")) && !strstr(info->model->name, xs("arms"))) {
            weapon_material->set_color(weapon_color);
            weapon_material->set_alpha(weapon_color.a);
            weapon_material->set_flag(MATERIAL_FLAG_IGNORE_Z, true);

            interfaces::model_render->force_material_override(weapon_material);
            draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
            interfaces::model_render->force_material_override(nullptr);
            return;
        }
>>>>>>> parent of 4203e40... meme hook hehe
    }

    if (is_arms) {
        arm_material->set_color(arm_color);
        arm_material->set_alpha(arm_color.a);
        arm_material->set_flag(MATERIAL_FLAG_IGNORE_Z, false);

<<<<<<< HEAD
        interfaces::model_render->force_material_override(arm_material);
        draw_model_execute_hk.call_original<decltype(&draw_model_execute)>(ecx, edx, ctx, state, info, matrix);
        interfaces::model_render->force_material_override(nullptr);
        return;
=======
            interfaces::model_render->force_material_override(arm_material);
            draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
            interfaces::model_render->force_material_override(nullptr);
            return;
        }
>>>>>>> parent of 4203e40... meme hook hehe
    }

    if (is_sleeve) {
        sleeve_material->set_color(sleeve_color);
        sleeve_material->set_alpha(sleeve_color.a);
        sleeve_material->set_flag(MATERIAL_FLAG_IGNORE_Z, false);

<<<<<<< HEAD
        interfaces::model_render->force_material_override(sleeve_material);
        draw_model_execute_hk.call_original<decltype(&draw_model_execute)>(ecx, edx, ctx, state, info, matrix);
        return;
=======
            interfaces::model_render->force_material_override(sleeve_material);
            draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
            return;
        };
>>>>>>> parent of 4203e40... meme hook hehe
    }

    if (interfaces::model_render->is_forced_material_override())
        return draw_model_execute_original(ecx, edx, ctx, state, info, matrix);

    // do any player model related chams below.

    if (info->flags != 1 || strstr(info->model->name, xs("models/player")) == nullptr) {
        draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
        return;
    }

    c_player *entity = (c_player *) interfaces::entity_list->get_entity(info->entity_index);

    const auto visible_color = settings.visuals.player.chams.visible_color;
    const auto invisible_color = settings.visuals.player.chams.invisible_color;

    if (entity == nullptr || !entity->is_enemy() || entity->get_life_state() != LIFE_STATE_ALIVE) {
        draw_model_execute_original(ecx, edx, ctx, state, info, matrix);

        return;
    }

    c_material *material = nullptr;

    switch (settings.visuals.player.chams.material) {
    case 0: material = textured; break;
    case 1: material = flat; break;
    }

    if (settings.visuals.player.chams.invisible) {
        material->set_color(invisible_color);
        material->set_alpha(invisible_color.a);
        material->set_flag(MATERIAL_FLAG_IGNORE_Z, true);

        interfaces::model_render->force_material_override(material);

        draw_model_execute_original(ecx, edx, ctx, state, info, matrix);

        if (!settings.visuals.player.chams.visible)
            interfaces::model_render->force_material_override(nullptr);
    }

    if (settings.visuals.player.chams.visible) {
        material->set_color(visible_color);
        material->set_alpha(visible_color.a);
        material->set_flag(MATERIAL_FLAG_IGNORE_Z, false);

        interfaces::model_render->force_material_override(material);

        draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
    } else {
        interfaces::model_render->force_material_override(nullptr);

        draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
    }

    const auto glow_color = settings.visuals.player.chams.glow_color;
    if (settings.visuals.player.chams.glow) {
        float rgba[] = {(float) glow_color.r / 255.f, (float) glow_color.g / 255.f, (float) glow_color.b / 255.f, (float) glow_color.a / 255.f};

        auto tint = glow->find_var(xs("$envmaptint"), nullptr);
        if (tint)
            tint->set(rgba[0], rgba[1], rgba[2]);

        auto alpha = glow->find_var(xs("$alpha"), nullptr);
        if (alpha)
            alpha->set(rgba[3]);

        auto envmap = glow->find_var(xs("$envmapfresnelminmaxexp"), nullptr);
        if (envmap)
            envmap->set(0.f, 1.f, 8.0f);

        interfaces::model_render->force_material_override(glow);
        draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
    }

    draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
    interfaces::model_render->force_material_override(nullptr);
}