#include <windows.h>

#include "core/cheat/cheat.h"
#include "engine/hash/hash.h"
#include "engine/input/input.h"
#include "engine/logging/logging.h"
#include "engine/pe/pe.h"
#include "engine/render/render.h"
#include "engine/security/xorstr.h"

// i love you, i love all of you, i love you chiddy for being that positive guy that used to always bring a smile to my face
// i love chawndi for always being here
// i love you dylan for showing me what it really means to love someone for who they really are
// i love you grassii for doing everything you could in spite of not understanding
// i love you sim for understanding me like noone else before
// i love you h0pde for being the greatest and most honest friend i ever met
// i love you hayden for being so light-hearted and cheerful
// i love you carol for showing me whats it like to care for someone with your life

// im in love with navewindre
// why is this still here
// im in love with laine

// fuck you duxe u pasting fucking retard i want to fucking slap your fucking mother so god damn hard for not drowning you at birth you stupid dumb fucking rat bastard im going to literally shut your fucking power off and then make ur fridge run down the street


// https://i.imgur.com/xD24aJu.jpg

// this cheats going to have my unironic suicide note in it one day

unsigned long __stdcall initial_thread(void *base_pointer) {
#ifdef _DEBUG
    logging::init(SEVERITY_DEBUG);
#else
    logging::init(SEVERITY_INFO);
#endif

    auto i = 0;

    if (!pe::init())
        goto load_failed;

    while (pe::get_module(XORSTR("serverbrowser.dll")) == 0u) {
        if (++i == 5) {
            logging::error(XORSTR("couldn't find the 'serverbrowser.dll' module"));

            goto load_failed;
        }

        Sleep(1000);
    }

    if (!cheat::init())
        goto load_failed;

#ifdef _DEBUG
    while (!input::is_key_down(VK_DELETE) || !input::is_key_down(VK_END))
        Sleep(100);
#endif

load_failed:
#ifdef _DEBUG
    cheat::undo();

    FreeLibraryAndExitThread((HMODULE) base_pointer, 0);
#endif

    return 0;
}

int __stdcall DllMain(void *base_pointer, unsigned int reason_to_call, void *reserved) {
    if (reason_to_call != 1)
        return 1;

#ifdef _DEBUG
    DisableThreadLibraryCalls((HMODULE) base_pointer);
    CreateThread(nullptr, 0, &initial_thread, base_pointer, 0, nullptr);
#else
    initial_thread(base_pointer);

#endif

    return 1;
}
