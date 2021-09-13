#include <string>
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../engine/security/xorstr.h"
#include "../ui/ui.h"

bool __fastcall hooks::draw_print_text(uintptr_t ecx, uintptr_t edx, const wchar_t *text, int text_length, int draw_type) {
    if (wcsncmp(text, xs(L"fps"), 3) == 0) {
        const color_t color = ui::get_accent_color();
        interfaces::surface->set_text_color(color.r, color.g, color.b, color.a);

        const auto millionware_string = std::wstring(xs(L"[millionware] "));
        draw_print_text_original(ecx, edx, millionware_string.data(), millionware_string.length(), draw_type);

        interfaces::surface->set_text_color(229, 229, 178, 255); // reset to default color
        return draw_print_text_original(ecx, edx, text, text_length, draw_type);
    }

    return draw_print_text_original(ecx, edx, text, text_length, draw_type);
}