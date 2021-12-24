#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"

void __fastcall hooks::shutdown(uintptr_t ecx, uintptr_t edx) {
    // purpose:
    // prevent unhandledExceptionFilter from
    // printing shit relating back to the
    // event listener when you close the game
    // can be used to prevent process 

    // inform other thread that we've closed the game
    cheat::panic = true;

    // call shutdown, resulting in the process closing, without our module present
    shutdown_original(ecx, edx);
}