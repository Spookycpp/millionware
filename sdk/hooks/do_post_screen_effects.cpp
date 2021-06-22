#include "../core/hooks/hooks.h"

int __fastcall hooks::do_post_screen_effects(c_client_mode *ecx, uintptr_t edx, int effect_id) {
    return do_post_screen_effects_original(ecx, edx, effect_id);
}
