#pragma once

#include "macros.h"

class c_hud_chat {
public:
    DECLARE_VFUNC(27, chat_printf(int player_index, int filter, const char *fmt), void(__cdecl *)(void *, int, int, const char *))
    (player_index, filter, fmt);
};

class c_client_mode {
public:
    char pad0001[28];
    c_hud_chat *chat; // 0x2A
};