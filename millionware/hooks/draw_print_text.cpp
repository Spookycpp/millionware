#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"

#include "../utils/util.hpp"

bool __fastcall hooks::draw_print_text_hook(uintptr_t ecx, uintptr_t edx, const wchar_t* text, int textlen, int drawtype)
{
    if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
        return hooks::draw_print_text.get_original<decltype(&draw_print_text_hook)>()(ecx, edx, text, textlen, drawtype);

    if (text[0] == 'f' &&
        text[1] == 'p' &&
        text[2] == 's') {
        std::wstring millionware_string = std::wstring(L"[millionware] ") + text;
        return hooks::draw_print_text.get_original<decltype(&draw_print_text_hook)>()(ecx, edx, millionware_string.data(), millionware_string.length(), drawtype);
    }
    return hooks::draw_print_text.get_original<decltype(&draw_print_text_hook)>()(ecx, edx, text, textlen, drawtype);
}