#pragma once

#include "macros.h"

enum e_game_type {
    GAME_TYPE_UNKNOWN = -1,
    GAME_TYPE_CLASSIC,
    GAME_TYPE_GUNGAME,
    GAME_TYPE_TRAINING,
    GAME_TYPE_CUSTOM,
    GAME_TYPE_COOPERATIVE,
    GAME_TYPE_SKIRMISH,
    GAME_TYPE_FREEFORALL
};

// https://developer.valvesoftware.com/wiki/CSGO_Game_Mode_Commands
enum e_game_mode {
    GAME_MODE_UNKNOWN = 0,
    GAME_MODE_CASUAL,
    GAME_MODE_COMPETITIVE,
    GAME_MODE_WINGMAN,
    GAME_MODE_ARMSRACE,
    GAME_MODE_DEMOLITION,
    GAME_MODE_DEATHMATCH,
    GAME_MODE_GUARDIAN,
    GAME_MODE_COOPSTRIKE,
    GAME_MODE_DANGERZONE
};

// https://github.com/scen/ionlib/blob/master/src/sdk/hl2_csgo/public/matchmaking/igametypes.h
class c_game_types {
public:
    DECLARE_VFUNC(8, get_current_game_type(), int(__thiscall *)(void *))();
    DECLARE_VFUNC(9, get_current_game_mode(), int(__thiscall *)(void *))();
    DECLARE_VFUNC(10, get_current_map_name(), const char *(__thiscall *) (void *) )();
    DECLARE_VFUNC(11, get_current_game_type_name(), const char *(__thiscall *) (void *) )();
    DECLARE_VFUNC(13, get_current_game_mode_name(), const char *(__thiscall *) (void *) )();
};