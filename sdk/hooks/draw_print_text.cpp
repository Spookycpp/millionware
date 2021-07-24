
#include <stdio.h>
#include <string.h>
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"

#include "../engine/security/xorstr.h"

bool __fastcall hooks::draw_print_text(uintptr_t ecx, uintptr_t edx, const wchar_t *text, int text_length, int draw_type) {

    if (wcsncmp(text, xs(L"fps"), 3) == 0) {
        std::wstring millionware_string = std::wstring(xs(L"[millionware] ")) + text;
        return draw_print_text_original(ecx, edx, millionware_string.data(), millionware_string.length(), draw_type);
    }

    return draw_print_text_original(ecx, edx, text, text_length, draw_type);
}