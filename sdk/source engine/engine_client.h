#pragma once

#include "macros.h"
#include "network_channel.h"
#include "vector.h"

class c_spatial_query;
class c_network_channel;

struct player_info_t {
	uint64_t version;
	uint64_t xuid;
	char name[128];
	int user_id;
	char guid[33];
	int friends_id;
	char friends_name[128];
	bool fake_player;
	bool hltv;

private:
	char padding_[25];
};

class c_engine_client {
public:
	DECLARE_VFUNC(5, get_screen_size(int& width, int& height), void(__thiscall*)(void*, int& width, int& height)) (width, height);
	DECLARE_VFUNC(8, get_player_info(int ent_index, player_info_t& buffer), bool(__thiscall*)(void*, int, player_info_t&)) (ent_index, buffer);
	DECLARE_VFUNC(9, get_player_for_user_id(int user_id), int(__thiscall*)(void*, int)) (user_id);
	DECLARE_VFUNC(12, get_local_player(), int(__thiscall*)(void*))();
	DECLARE_VFUNC(18, get_view_angles(vector_t& angles), void(__thiscall*) (void*, vector_t&)) (angles);
	DECLARE_VFUNC(19, set_view_angles(const vector_t& angles), void(__thiscall*) (void*, const vector_t&)) (angles);
	DECLARE_VFUNC(26, is_in_game(), bool(__thiscall*) (void*))();
	DECLARE_VFUNC(27, is_connected(), bool(__thiscall*) (void*))();
	DECLARE_VFUNC(43, get_bsp_tree_query(), c_spatial_query* (__thiscall*)(void*))();
	DECLARE_VFUNC(78, get_net_channel_info(), c_net_channel_info* (__thiscall*)(void*))();
	DECLARE_VFUNC(108, execute_command(const char* command), void(__thiscall*)(void*, const char*)) (command);
    DECLARE_VFUNC(104, get_engine_build_number(), int(__thiscall *)(void *))()
};
