#pragma once

#include "color.h"
#include "macros.h"

class c_convar
{
public:
	c_convar* next;

private:
	char padding0_[12];

public:
	int flags;

private:
	char padding1_[4];

public:
	c_convar* parent;

private:
	char padding2_[36];

public:
	DECLARE_VFUNC(12, get_float(), float(__thiscall *)(void *))();
	DECLARE_VFUNC(13, get_int(), int(__thiscall *)(void *))();
	DECLARE_VFUNC(13, get_bool(), bool(__thiscall *)(void *))();
	DECLARE_VFUNC(14, set_value(const char *value), void(__thiscall *)(void *, const char *))(value);
	DECLARE_VFUNC(15, set_value(float value), void(__thiscall *)(void *, float))(value);
	DECLARE_VFUNC(16, set_value(int value), void(__thiscall *)(void *, int))(value);
	DECLARE_VFUNC(16, set_value(bool value), void(__thiscall *)(void *, int))((int) value);

	int& get_flags() {
		return flags;
	}
};

class c_cvar
{
public:
	DECLARE_VFUNC(16, find_convar(const char *key), c_convar *(__thiscall *)(void *, const char *))(key);
};
