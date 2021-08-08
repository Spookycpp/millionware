#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <windows.h>

#include "core/cheat/cheat.h"
#include "engine/hash/hash.h"
#include "engine/input/input.h"
#include "engine/logging/logging.h"
#include "engine/pe/pe.h"
#include "engine/render/render.h"
#include "engine/security/xorstr.h"

#include <DbgHelp.h>
#include <winternl.h>
#include <locale>
#include <codecvt>
#include <format>

// shoutout navewindre 4 dis one
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

// saves us fps
// doesnt matter theyre fucked anyways

// laines diary don't open.
// day 1, czapek has drove me insane - day 1 pt2, czapek is fail pasting view_matrix
// day 2, naz has been driving me insane that nigga makes me wanna throw a toaster into his mothers bathtub
// day 3, eternity still hasnt made any progress on the loader and is sending me random webm memes instead of answering stuff
// day 9, can't figure out why the loader is crashing, last ditch effort will be asking duxe
// day 10, loader has been fixed. millionware is a go.
// day 32, cheats kinda coming together, still don't wanna release it or do the skin changer
// day 39, eternity is back.

//⠀⠀⠀⠀⠀⠀⠀⣠⣤⣤⣤⣤⣤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  
//⠀⠀⠀⠀⠀⢰⡿⠋⠁⠀⠀⠈⠉⠙⠻⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  
//⠀⠀⠀⠀⢀⣿⠇⠀⢀⣴⣶⡾⠿⠿⠿⢿⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  
//⠀⠀⣀⣀⣸⡿⠀⠀⢸⣿⣇⠀⠀⠀⠀⠀⠀⠙⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  
//⠀⣾⡟⠛⣿⡇⠀⠀⢸⣿⣿⣷⣤⣤⣤⣤⣶⣶⣿⠇⠀⠀⠀⠀⠀⠀⠀⣀⠀⠀  
//⢀⣿⠀⢀⣿⡇⠀⠀⠀⠻⢿⣿⣿⣿⣿⣿⠿⣿⡏⠀⠀⠀⠀⢴⣶⣶⣿⣿⣿⣆  
//⢸⣿⠀⢸⣿⡇⠀⠀⠀⠀⠀⠈⠉⠁⠀⠀⠀⣿⡇⣀⣠⣴⣾⣮⣝⠿⠿⠿⣻⡟  
//⢸⣿⠀⠘⣿⡇⠀⠀⠀⠀⠀⠀⠀⣠⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠁⠉⠀  
//⠸⣿⠀⠀⣿⡇⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠉⠀⠀⠀⠀  
//⠀⠻⣷⣶⣿⣇⠀⠀⠀⢠⣼⣿⣿⣿⣿⣿⣿⣿⣛⣛⣻⠉⠁⠀⠀⠀⠀⠀⠀⠀  
//⠀⠀⠀⠀⢸⣿⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀  
//⠀⠀⠀⠀⢸⣿⣀⣀⣀⣼⡿⢿⣿⣿⣿⣿⣿⡿⣿⣿⣿


static void *cheat_module_base = 0;

static std::pair<uint64_t, std::string> get_containing_module(uint64_t address) {
    struct _LDR_DATA_TABLE_ENTRY_ {
        PVOID Reserved1[2];
        LIST_ENTRY InMemoryOrderLinks;
        PVOID Reserved2[2];
        PVOID DllBase;
        PVOID Reserved3[2];
        UNICODE_STRING FullDllName;
        UNICODE_STRING BaseDllName;
    };

    const auto peb = reinterpret_cast<PEB *>(reinterpret_cast<TEB *>(__readfsdword(0x18))->ProcessEnvironmentBlock);
    const auto module_list = &peb->Ldr->InMemoryOrderModuleList;

    for (auto i = module_list->Flink; i != module_list; i = i->Flink) {
        auto entry = CONTAINING_RECORD(i, _LDR_DATA_TABLE_ENTRY_, InMemoryOrderLinks);

        if (!entry)
            continue;

        const auto module_start = (uint64_t) entry->DllBase;
        const auto dos_headers = (IMAGE_DOS_HEADER *) module_start;
        const auto nt_headers = (IMAGE_NT_HEADERS *) (module_start + dos_headers->e_lfanew);

        if (address >= module_start && address <= module_start + nt_headers->OptionalHeader.SizeOfImage) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

            return std::make_pair(module_start, converter.to_bytes(entry->BaseDllName.Buffer, entry->BaseDllName.Buffer + entry->BaseDllName.Length / 2));
        }
    }

    MEMORY_BASIC_INFORMATION mbi;

    VirtualQuery(cheat_module_base, &mbi, sizeof(mbi));

    if ((DWORD64) address >= (DWORD64) mbi.BaseAddress && (DWORD64) address <= (DWORD64) mbi.BaseAddress + mbi.RegionSize) {
        return std::make_pair((uint64_t) mbi.BaseAddress, xs("<cheat>"));
    }
    else {
        return std::make_pair((uint64_t) mbi.BaseAddress, std::format(xs("<unknown module {:#x}>"), (DWORD64) mbi.BaseAddress));
    }

    return std::make_pair(0, xs("<unknown module>"));
}

long __stdcall unhandledExceptionFilter(EXCEPTION_POINTERS *info) {

    if (info->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
        return EXCEPTION_CONTINUE_SEARCH;

    DWORD64 displacement;

    const auto symbol = (SYMBOL_INFO *) std::malloc(sizeof(SYMBOL_INFO) + 256);

    if (symbol != nullptr) {
        const auto [module_base, module_name] = get_containing_module((uint64_t) info->ExceptionRecord->ExceptionAddress);

        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = 255;

        std::string symbol_info;

        if (SymFromAddr(GetCurrentProcess(), (DWORD64) info->ExceptionRecord->ExceptionAddress, &displacement, symbol)) {
            symbol_info = std::format(xs("{}!{} + {:#x}"), module_name, symbol->Name, displacement);
        }
        else {
            symbol_info = std::format(xs("{} + {:#x}"), module_name, (DWORD64) info->ExceptionRecord->ExceptionAddress - module_base);
        }

        auto message = std::format(xs("Exception code: {:#x}\nException information: {}\n"), (uintptr_t) info->ExceptionRecord->ExceptionCode, symbol_info);

        if (info->ContextRecord->Ebp != 0) {
            message += xs("\n");

            auto ebp = info->ContextRecord->Ebp;

            while (true) {
                const auto eip_ebp = ebp + 4;
                const auto eip = *(uint32_t *) eip_ebp;

                if (eip == 0)
                    break;

                ebp = *(uint32_t *) ebp;

                const auto [module_base, module_name] = get_containing_module((uint64_t) eip);

                symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                symbol->MaxNameLen = 255;

                if (SymFromAddr(GetCurrentProcess(), (DWORD64) eip, &displacement, symbol)) {
                    message += std::format(xs("> {}!{} + {:#x}\n"), module_name, symbol->Name, displacement);
                }
                else {
                    message += std::format(xs("> {} + {:#x}\n"), module_name, (DWORD64) eip - module_base);
                }
            }
        }

        message += xs("\nCopy this information using CTRL+C and report it to the developers");

        MessageBoxA(nullptr, message.data(), nullptr, MB_ICONERROR | MB_OK);
        ExitProcess(0);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

unsigned long __stdcall initial_thread(void *base_pointer) {
    cheat_module_base = base_pointer;

    AddVectoredExceptionHandler(true, unhandledExceptionFilter);
    SymInitialize(GetCurrentProcess(), nullptr, true);

    logging::init();

    auto i = 0;

    while (pe::get_module(xs("serverbrowser.dll")) == 0u) {
        if (++i == 5) {
            logging::error(xs("couldn't find the 'serverbrowser.dll' module"));
    
            goto load_failed;
        }
    
        Sleep(1000);
    }
    
    if (!cheat::init())
        goto load_failed;

#ifdef _DEBUG
    while (!input::is_key_down(VK_DELETE) || !input::is_key_down(VK_END))
        Sleep(100);
#else
    while (true)
        Sleep(100);
#endif

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
