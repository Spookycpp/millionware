#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"

#include "../engine/security/xorstr.h"

#include "../engine/logging/logging.h"
#include "../core/cheat/cheat.h"

void __fastcall hooks::push_notice(uintptr_t ecx, uintptr_t edx, const char *text, int len, const char *panel) {
    if (text == "##" + cheat::notice_text) {
        push_notice_original(ecx, edx, cheat::notice_text.c_str(), cheat::notice_text.length(), cheat::notice_text.c_str());
        return;
    }

    push_notice_original(ecx, edx, text, len, panel);
}