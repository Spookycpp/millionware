#pragma once

#include "macros.h"
#include "network_channel.h"
#include "vector.h"

class c_spatial_query;
class c_network_channel;

struct player_info_t
{
private:
	char padding0[8];

public:
	union
	{
		unsigned long long xuid;

		struct
		{
			int xuid_low;
			int xuid_high;
		};
	};

	char name[128];
	int user_id;
	char guid[33];
	int friends_id;
	char friends_name[128];
	bool fake_player;
	bool hltv;
	char padding1[17];
};

class c_engine_client
{
public:
	DECLARE_VFUNC(5, get_screen_size(int &width, int &height), void(__thiscall *)(void *, int &width, int &height)) (width, height);
	DECLARE_VFUNC(8, get_player_info(int ent_index, player_info_t &buffer), void(__thiscall *)(void *, int, player_info_t &)) (ent_index, buffer);
	DECLARE_VFUNC(9, user_id_to_ent_index(int user_id), int(__thiscall *)(void *, int)) (user_id);
	DECLARE_VFUNC(12, get_local_player(), int(__thiscall *)(void *))();
	DECLARE_VFUNC(18, get_view_angles(vector_t &angles), void(__thiscall *) (void *, vector_t &)) (angles);
	DECLARE_VFUNC(19, set_view_angles(const vector_t &angles), void(__thiscall *) (void *, const vector_t &)) (angles);
	DECLARE_VFUNC(26, is_in_game(), bool(__thiscall *) (void *))();
	DECLARE_VFUNC(27, is_connected(), bool(__thiscall *) (void *))();
	DECLARE_VFUNC(43, get_bsp_tree_query(), c_spatial_query *(__thiscall *)(void *))();
	DECLARE_VFUNC(78, get_net_channel_info(), c_net_channel_info *(__thiscall *)(void *))();
	DECLARE_VFUNC(108, execute_command(const char *command), void(__thiscall *)(void *, const char *)) (command);
};
