#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../engine/hash/hash.h"

void __fastcall hooks::screen_size_changed(uintptr_t ecx, uintptr_t edx, int old_width, int old_height) {
    screen_size_changed_original(ecx, edx, old_width, old_height);

    // surface::refresh_fonts();
}