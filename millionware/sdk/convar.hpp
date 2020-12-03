#pragma once

#include "macros.hpp"
#include "typedefs.hpp"
#include "valve_vector.hpp"

enum e_convar_flag {
	CONVAR_FLAG_NONE = 0,
	CONVAR_FLAG_UNREGISTERED = 1 << 0,
	CONVAR_FLAG_DEVELOPMENTONLY = 1 << 1,
	CONVAR_FLAG_GAMEDLL = 1 << 2,
	CONVAR_FLAG_CLIENTDLL = 1 << 3,
	CONVAR_FLAG_HIDDEN = 1 << 4,
	CONVAR_FLAG_PROTECTED = 1 << 5,
	CONVAR_FLAG_SPONLY = 1 << 6,
	CONVAR_FLAG_ARCHIVE = 1 << 7,
	CONVAR_FLAG_NOTIFY = 1 << 8,
	CONVAR_FLAG_USERINFO = 1 << 9,
	CONVAR_FLAG_CHEAT = 1 << 14,
	CONVAR_FLAG_PRINTABLEONLY = 1 << 10,
	CONVAR_FLAG_UNLOGGED = 1 << 11,
	CONVAR_FLAG_NEVER_AS_STRING = 1 << 12,
	CONVAR_FLAG_REPLICATED = 1 << 13,
	CONVAR_FLAG_DEMO = 1 << 16,
	CONVAR_FLAG_DONTRECORD = 1 << 17,
	CONVAR_FLAG_RELOAD_MATERIALS = 1 << 20,
	CONVAR_FLAG_RELOAD_TEXTURES = 1 << 21,
	CONVAR_FLAG_NOT_CONNECTED = 1 << 22,
	CONVAR_FLAG_MATERIAL_SYSTEM_THREAD = 1 << 23,
	CONVAR_FLAG_ARCHIVE_XBOX = 1 << 24,
	CONVAR_FLAG_ACCESSIBLE_FROM_THREADS = 1 << 25,
	CONVAR_FLAG_SERVER_CAN_EXECUTE = 1 << 28,
	CONVAR_FLAG_SERVER_CANNOT_QUERY = 1 << 29,
	CONVAR_FLAG_CLIENTCMD_CAN_EXECUTE = 1 << 30,
};

class c_convar {
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
	c_valve_vector<fn_change_callback_t> callbacks;

	VIRTUAL_METHOD(const char*, get_name, 6, ());
	VIRTUAL_METHOD(float, get_float, 12, ());
	VIRTUAL_METHOD(int, get_int, 13, ());
	VIRTUAL_METHOD(void, set_value, 14, (std::string_view value), value.data());
	VIRTUAL_METHOD(void, set_value, 15, (float value), value);
	VIRTUAL_METHOD(void, set_value, 16, (int value), value);
};
