#pragma once
#include "game_events.h"

class c_game_event_manager {
  public:
    DECLARE_VFUNC(3, add_listener(void *listener, const char *event_name, bool server_side), bool(__thiscall *)(void *, void *, const char *, bool))(listener, event_name, server_side);
    DECLARE_VFUNC(5, remove_listener(void *listener), bool(__thiscall *)(void *, void *))(listener);
};