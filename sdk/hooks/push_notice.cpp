#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"

#include "../engine/debug/debug_overlay.h"
#include "../engine/security/xorstr.h"

#include "../core/cheat/cheat.h"
#include "../engine/logging/logging.h"

void __fastcall hooks::push_notice(uintptr_t ecx, uintptr_t edx, const char *text, int len, const char *panel) {
    PROFILE_WITH(push_notice);

    if (text == "##" + cheat::notice_text) {
        push_notice_hk.call_original<decltype(&push_notice)>(ecx, edx, cheat::notice_text.c_str(), cheat::notice_text.length(), cheat::notice_text.c_str());
        return;
    }

    push_notice_hk.call_original<decltype(&push_notice)>(ecx, edx, text, len, panel);
}