#pragma once

#include "macros.h"

class c_vgui_panel
{
public:
	DECLARE_VFUNC(31, set_keyboard_input_enabled(int panel_id, bool state), void(__thiscall *)(void *, int, bool))(panel_id, state);
	DECLARE_VFUNC(32, set_mouse_input_enabled(int panel_id, bool state), void(__thiscall *)(void *, int, bool))(panel_id, state);
	DECLARE_VFUNC(36, get_panel_name(int panel_id), const char *(__thiscall *)(void *, int))(panel_id);
};

class c_vgui_surface
{
public:
	DECLARE_VFUNC(66, unlock_cursor(), void(__thiscall *)(void *))();
	DECLARE_VFUNC(67, lock_cursor(), void(__thiscall *)(void *))();
};
