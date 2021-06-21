#include <windows.h>

#include "core/cheat/cheat.h"
#include "engine/hash/hash.h"
#include "engine/input/input.h"
#include "engine/logging/logging.h"
#include "engine/pe/pe.h"
#include "engine/render/render.h"
#include "engine/security/xorstr.h"



// i love you bowish for falling for my lie that my girlfriend had died >.<
// i love you swoopae for giving me the gift of edebug

// im in love with navewindre
// why is this still here
// im in love with laine

// fuck you duxe u pasting fucking retard i want to fucking slap your fucking mother so god damn hard for not drowning you at birth you stupid dumb fucking rat bastard im going to literally shut your
// fucking power off and then make ur fridge run down the street

// https://i.imgur.com/xD24aJu.jpg

// this cheats going to have my unironic suicide note in it one day

// if this source gets leaked, this is all of our real notes, this is from no one else this is purely our thoughts and nothing else why would u even remotely suspect that we would be lying about such
// things you are a fake and a lame and a snake and a opp u gonna get smoked on ur own set if you keep talking shit im going to send duxe after you with his pink egirl gaming chair and he'll run u
// over in his bmw that he somehow bought from pasting a dogshit rust hack that fucking bluescreened my pc 9 times in 1 sitting using that shitty remote desktop bypass as well fucking aids as fuck
// shoutout daum 4 the cerb bypass tho was fun raging w/o any fkn bans for like a week str8 miss those days ngl
// nigga

// i should be writing this hack but i can't think of anything to do rn i cbf to finish inventory changer man
// gonna be on a fbi watchlist if this gets out ngl

/* credits: duxe/laine/aiden */

// laines diary don't open.
// day 1, czapek has drove me insane - day 1 pt2, czapek is fail pasting view_matrix
// day 2, naz has been driving me insane that nigga makes me wanna throw a toaster into his mothers bathtub
// day 3, eternity still hasnt made any progress on the loader and is sending me random webm memes instead of answering stuff
unsigned long __stdcall initial_thread(void *base_pointer) {
#ifdef _DEBUG
    logging::init(SEVERITY_DEBUG);
#else
    logging::init(SEVERITY_INFO);
#endif

    auto i = 0;

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
