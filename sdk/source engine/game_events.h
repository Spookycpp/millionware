#pragma once

#include "macros.h"

class c_game_event
{
public:
	DECLARE_VFUNC(1, get_name(), const char *(__thiscall *)(void *))();
	DECLARE_VFUNC(2, is_reliable(), bool(__thiscall *)(void *))();
	DECLARE_VFUNC(3, is_local(), bool(__thiscall *)(void *))();
	DECLARE_VFUNC(4, is_empty(const char *key), bool(__thiscall *)(void *, const char *))(key);
	DECLARE_VFUNC(5, get_bool(const char *key, bool default_value = false), bool(__thiscall *)(void *, const char *, bool))(key, default_value);
	DECLARE_VFUNC(6, get_int(const char *key, int default_value = 0), int(__thiscall *)(void *, const char *, int))(key, default_value);
	DECLARE_VFUNC(7, get_long(const char *key, unsigned long default_value = 0), unsigned long(__thiscall *)(void *, const char *, unsigned long))(key, default_value);
	DECLARE_VFUNC(8, get_float(const char *key, float default_value = 0.0f), float(__thiscall *)(void *, const char *, float))(key, default_value);
	DECLARE_VFUNC(9, get_string(const char *key, const char *default_value = ""), const char *(__thiscall *)(void *, const char *, const char *))(key, default_value);
	DECLARE_VFUNC(10, get_wstring(const char *key, const wchar_t *default_value = L""), const wchar_t *(__thiscall *)(void *, const char *, const wchar_t *))(key, default_value);
	DECLARE_VFUNC(11, set_bool(const char *key, bool value), void(__thiscall *)(void *, const char *, bool))(key, value);
	DECLARE_VFUNC(12, set_int(const char *key, int value), void(__thiscall *)(void *, const char *, int))(key, value);
	DECLARE_VFUNC(13, set_long(const char *key, unsigned long value), void(__thiscall *)(void *, const char *, unsigned long))(key, value);
	DECLARE_VFUNC(14, set_float(const char *key, float value), void(__thiscall *)(void *, const char *, float))(key, value);
	DECLARE_VFUNC(15, set_string(const char *key, const char *value), void(__thiscall *)(void *, const char *, const char *))(key, value);
	DECLARE_VFUNC(16, set_wstring(const char *key, const wchar_t *value), void(__thiscall *)(void *, const char *, const wchar_t *))(key, value);
};
