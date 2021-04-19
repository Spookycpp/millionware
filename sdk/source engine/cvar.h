#pragma once

#include "color.h"
#include "macros.h"

class c_convar
{
public:
	DECLARE_VFUNC(12, get_float(), float(__thiscall *)(void *))();
	DECLARE_VFUNC(13, get_int(), int(__thiscall *)(void *))();
	DECLARE_VFUNC(13, get_bool(), bool(__thiscall *)(void *))();
	DECLARE_VFUNC(14, set_value(const char *value), void(__thiscall *)(void *, const char *))(value);
	DECLARE_VFUNC(15, set_value(float value), void(__thiscall *)(void *, float))(value);
	DECLARE_VFUNC(16, set_value(int value), void(__thiscall *)(void *, int))(value);
	DECLARE_VFUNC(16, set_value(bool value), void(__thiscall *)(void *, int))((int) value);

	char* get_default() const {
		return _default;
	}

	const char* get_string() const {
		return _string;
	}

	int& get_flags() {
		return _flags;
	}

private:
	void         *_vft;
	c_convar*	  _next;            // 0x0004
	__int32       _registered;      // 0x0008
	char         *_name;            // 0x000C
	char         *_help_str;        // 0x0010
	__int32       _flags;           // 0x0014
	char pad[0x4];
	c_convar*	  _parent;          // 0x001C
	char         *_default;         // 0x0020
	char         *_string;          // 0x0024
	__int32       _str_len;         // 0x0028
	float         _float_val;       // 0x002C
	__int32       _int_val;         // 0x0030
	__int32       _has_min;         // 0x0034
	float         _min;             // 0x0038
	__int32       _has_max;         // 0x003C
	float         _max;             // 0x0040
	void         *_change_callback; // 0x0044
};

class c_cvar
{
public:
	DECLARE_VFUNC(16, find_convar(const char *key), c_convar *(__thiscall *)(void *, const char *))(key);
};
