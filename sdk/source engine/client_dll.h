#pragma once

#include "macros.h"

class c_client_class;
class c_client_mode;
class c_client_state;
class c_entity;

enum e_client_frame_stage {
	FRAME_STAGE_UNDEFINED = -1,
	FRAME_STAGE_START,
	FRAME_STAGE_NET_UPDATE_START,
	FRAME_STAGE_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_STAGE_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_STAGE_NET_UPDATE_END,
	FRAME_STAGE_RENDER_START,
	FRAME_STAGE_RENDER_END
};

class c_base_client_dll
{
public:
	DECLARE_VFUNC(8, get_all_classes(), c_client_class *(__thiscall *)(void *))();
	DECLARE_VFUNC(38, dispatch_user_msg(int msg_type, int arg1, int length, void *data = nullptr), bool(__thiscall *)(void *, int, int, int, void *))(msg_type, arg1, length, data);
};

class c_entity_list
{
public:
	DECLARE_VFUNC(3, get_entity(int index), c_entity *(__thiscall *) (void *, int))(index);
	DECLARE_VFUNC(4, get_entity_from_handle(uintptr_t handle), c_entity *(__thiscall *)(void *, uintptr_t))(handle);
	DECLARE_VFUNC(6, get_highest_ent_index(), int(__thiscall *)(void *))();
};
