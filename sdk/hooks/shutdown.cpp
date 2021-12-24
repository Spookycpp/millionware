#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"

void __fastcall hooks::shutdown(uintptr_t ecx, uintptr_t edx) {
    // moneybot.cc/onetap.com (v1).

    // prevent unhandledExceptionFilter from printing shit relating back to the event listener when you close the game.
    // also can be used to prevent process from hanging, crashing or in some cases dumping.

    // inform other thread that we've closed the game.
    cheat::panic = true;

    // call shutdown, resulting in the process closing, without our module present.
    shutdown_original(ecx, edx);
}