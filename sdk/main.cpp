#include <windows.h>

#include "core/cheat/cheat.h"
#include "engine/hash/hash.h"
#include "engine/input/input.h"
#include "engine/logging/logging.h"
#include "engine/pe/pe.h"
#include "engine/render/render.h"
#include "engine/security/xorstr.h"

#include <DbgHelp.h>

// i love you, i love all of you, i love you chiddy for being that positive guy that used to always bring a smile to my face
// i love chawndi for always being here
// i love you dylan for showing me what it really means to love someone for who they really are
// i love you grassii for doing everything you could in spite of not understanding
// i love you sim for understanding me like noone else before
// i love you h0pde for being the greatest and most honest friend i ever met
// i love you hayden for being so light-hearted and cheerful
// i love you carol for showing me whats it like to care for someone with your life
// i love you bowish for falling for my lie that my girlfriend had died >.<
// i love you swoopae for giving me the gift of ebog

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
// day 9, can't figure out why the loader is crashing, last ditch effort will be asking duxe
// day 10, loader has been fixed. millionware is a go.

static void* cheat_base = 0;

long __stdcall exception_filter(EXCEPTION_POINTERS *info) {
    if (info->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
        return EXCEPTION_EXECUTE_HANDLER;

    char buffer[512];

    std::string symbol_info;

    auto displacement = (DWORD64) 0;
    auto symbol = (SYMBOL_INFO *) calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);

    symbol->MaxNameLen = 255; // liar.
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    if (SymFromAddr(GetCurrentProcess(), (DWORD64) info->ExceptionRecord->ExceptionAddress, &displacement, symbol)) {
        symbol_info = fmt::format(XORSTR("{} + {:#x}"), symbol->Name, displacement);
    }
    else if ((DWORD64) info->ExceptionRecord->ExceptionAddress >= (DWORD64) cheat_base) {
        MEMORY_BASIC_INFORMATION mbi;

        VirtualQuery(cheat_base, &mbi, sizeof(mbi));

        if ((DWORD64) info->ExceptionRecord->ExceptionAddress <= (DWORD64) cheat_base + mbi.RegionSize) {
            symbol_info = fmt::format(XORSTR("cheat + {:#x}"), (DWORD64) info->ExceptionRecord->ExceptionAddress - (DWORD64) cheat_base);
        }
    }

    if (!symbol_info.empty()) {
        sprintf_s(buffer, XORSTR("Exception code: 0x%08x\nException address: 0x%08x\n\nSymbol information: %s\n\nCopy this information using CTRL+C and report it to developers"),
                  (uintptr_t) info->ExceptionRecord->ExceptionCode, (uintptr_t) info->ExceptionRecord->ExceptionAddress, symbol_info.data());
    }
    else {
        sprintf_s(buffer, XORSTR("Exception code: 0x%08x\nException address: 0x%08x\n\nCopy this information using CTRL+C and report it to developers"), (uintptr_t) info->ExceptionRecord->ExceptionCode,
                  (uintptr_t) info->ExceptionRecord->ExceptionAddress);
    }

    MessageBoxA(nullptr, buffer, nullptr, MB_ICONERROR | MB_OK);

    return EXCEPTION_EXECUTE_HANDLER;
}

unsigned long __stdcall initial_thread(void *base_pointer) {
    cheat_base = base_pointer;
    AddVectoredExceptionHandler(true, exception_filter);
    SymInitialize(GetCurrentProcess(), nullptr, true);

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
    while (true)
        Sleep(100);

load_failed:
#ifdef _DEBUG
    cheat::undo();
    SymCleanup(GetCurrentProcess());
    FreeLibraryAndExitThread((HMODULE) base_pointer, 0);
#endif

    return 0;
}

int __stdcall DllMain(HMODULE base_pointer, unsigned int reason_to_call, void *) {
    if (reason_to_call == 1) {

#ifdef _DEBUG
        DisableThreadLibraryCalls((HMODULE) base_pointer);
        CreateThread(nullptr, 0, &initial_thread, base_pointer, 0, nullptr);
#else
        initial_thread(base_pointer);
#endif
    }
    return 1;
}
