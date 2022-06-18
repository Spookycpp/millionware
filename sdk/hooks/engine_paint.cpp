#include "../core/hooks/hooks.h"

void __fastcall hooks::engine_paint(uintptr_t ecx, uintptr_t edx, int mode) {

    engine_paint_original(ecx, edx, mode);
}
