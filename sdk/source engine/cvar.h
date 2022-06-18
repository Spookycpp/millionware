#pragma once

#include "color.h"
#include "macros.h"
#include "util_vector.h"

enum e_convar_flags {
    CVAR_NONE = 0,
    CVAR_UNREGISTERED = (1 << 0),
    CVAR_DEVELOPMENT_ONLY = (1 << 1),
    CVAR_GAME_DLL = (1 << 2),
    CVAR_CLIENT_DLL = (1 << 3),
    CVAR_HIDDEN = (1 << 4),
    CVAR_PROTECTED = (1 << 5),
    CVAR_SPONLY = (1 << 6),
    CVAR_ARCHIVE = (1 << 7),
    CVAR_NOTIFY = (1 << 8),
    CVAR_USER_INFO = (1 << 9),
    CVAR_CHEAT = (1 << 14),
    CVAR_PRINTABLE_ONLY = (1 << 10),
    CVAR_UNLOGGED = (1 << 11),
    CVAR_NEVER_AS_STRING = (1 << 12),
    CVAR_REPLICATED = (1 << 13),
    CVAR_DEMO = (1 << 16),
    CVAR_DONT_RECORD = (1 << 17),
    CVAR_NOT_CONNECTED = (1 << 22),
    CVAR_ARCHIVE_XBOX = (1 << 24),
    CVAR_SERVER_CAN_EXECUTE = (1 << 28),
    CVAR_SERVER_CANNOT_QUERY = (1 << 29),
    CVAR_CLIENT_CMD_CAN_EXECUTE = (1 << 30)
};

using change_callback_t = void(__cdecl *)(void *var, const char *old, float flold);

class c_convar {
public:
    DECLARE_VFUNC(12, get_float(), float(__thiscall *)(void *))();
    DECLARE_VFUNC(13, get_int(), int(__thiscall *)(void *))();
    DECLARE_VFUNC(13, get_bool(), bool(__thiscall *)(void *))();
    DECLARE_VFUNC(14, set_value(const char *value), void(__thiscall *)(void *, const char *))(value);
    DECLARE_VFUNC(15, set_value(float value), void(__thiscall *)(void *, float))(value);
    DECLARE_VFUNC(16, set_value(int value), void(__thiscall *)(void *, int))(value);
    DECLARE_VFUNC(16, set_value(bool value), void(__thiscall *)(void *, int))((int) value);

private:
    void *vtable; // 0x0000

public:
    c_convar *next;     // 0x0004
    __int32 registered; // 0x0008
    char *name;         // 0x000C
    char *help_str;     // 0x0010
    __int32 flags;      // 0x0014
    char pad[0x4];
    c_convar *parent;    // 0x001C
    char *default_value; // 0x0020
    char *string;        // 0x0024
    __int32 str_len;     // 0x0028
    float float_val;     // 0x002C
    __int32 int_val;     // 0x0030
    __int32 has_min;     // 0x0034
    float min;           // 0x0038
    __int32 has_max;     // 0x003C
    float max;           // 0x0040
    CUtlVector<change_callback_t> callbacks;
};

class c_cvar {
public:
    DECLARE_VFUNC(16, find_convar(const char *key), c_convar *(__thiscall *) (void *, const char *) )(key);
};
