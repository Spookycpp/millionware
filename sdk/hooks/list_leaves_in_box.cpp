#include <intrin.h>

#include "../core/hooks/hooks.h"
#include "../core/patterns/patterns.h"
#include "../core/settings/settings.h"

#include "../source engine/leaf_system.h"

struct renderable_info_t {
    i_client_renderable *renderable;
    void *alpha_property;
    int enum_count;
    int render_frame;
    unsigned short first_shadow;
    unsigned short leaf_list;
    short area;
    uint16_t flags;
    uint16_t flags_other;
    vector_t bloated_abs_mins;
    vector_t bloated_abs_maxs;
    vector_t abs_mins;
    vector_t abs_maxs;
    int pad;
};

int __fastcall hooks::list_leaves_in_box(uintptr_t ecx, uintptr_t edx, const vector_t &mins, const vector_t &maxs, void *leaves_list,
                                         int max_leaves) {

    // static auto insert_into_tree_addr = patterns::get_insert_into_tree_list_leaves_in_box_call() + 0x5;
    //
    // if (!settings.visuals.player.chams.invisible)
    //    return list_leaves_in_box_original(ecx, edx, mins, maxs, leaves_list, max_leaves);
    //
    // if (uintptr_t(_ReturnAddress()) != insert_into_tree_addr)
    //    return list_leaves_in_box_original(ecx, edx, mins, maxs, leaves_list, max_leaves);
    //
    // const auto info = *reinterpret_cast<renderable_info_t **>(uintptr_t(_AddressOfReturnAddress()) + 0x14);
    //
    // if (!info || !info->renderable)
    //    return list_leaves_in_box_original(ecx, edx, mins, maxs, leaves_list, max_leaves);
    //
    // c_entity *ent = info->renderable->get_i_client_unknown()->get_base_entity();
    //
    // if (!ent || !ent->is_player())
    //    return list_leaves_in_box_original(ecx, edx, mins, maxs, leaves_list, max_leaves);
    //
    // info->flags &= ~0x100;
    // info->flags_other |= 0xC0;
    //
    // const auto map_min = vector_t{-16384.f, -16384.f, -16384.f};
    // const auto map_max = vector_t{16384.f, 16384.f, 16384.f};

    return list_leaves_in_box_original(ecx, edx, mins, maxs, leaves_list, max_leaves);
}