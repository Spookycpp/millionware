#pragma once

#include "macros.h"
#include "vector.h"

class c_client_class;
class c_client_mode;
class c_entity;

enum class e_client_frame_stage {
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


struct c_net_channel {
	char padding0_[20];
	bool processing_messages; //0x0014
	bool should_delete;		  //0x0015
	char padding1_[2];
	int out_sequence_num;	  //0x0018 last send outgoing sequence number
	int in_sequence_num;	  //0x001C last received incoming sequnec number
	int out_seq_ack;		  //0x0020 last received acknowledge outgoing sequnce number
	int out_reliable_state;   //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int in_reliable_state;    //0x0028 state of incoming reliable data
	int choked_packets;		  //0x002C number of choked packets
	float clear_time;
	float time_out;
	char name[32];

	char padding2_[1044];

	DECLARE_VFUNC(47, transmit(bool onlyreliable), float(__thiscall*)(void*, bool))(onlyreliable);

};

struct c_clock_drift_mgr {
	float clock_offsets[17];
	uint32_t cur_clock_offset;
	uint32_t server_tick;
	uint32_t client_tick;
};

struct c_client_state {
	char pad_0000[0x9C];
	c_net_channel* net_channel;
	uint32_t challenge_nr;
	char pad_00A4[0x64];
	uint32_t sign_on_state;
	char pad_010C[0x8];
	float next_cmd_time;
	uint32_t server_count;
	uint32_t current_sequence;
	char pad_0120[0x04];
	c_clock_drift_mgr clock_drift_mgr;
	int32_t delta_tick;
	bool paused;
	char pad_0179[0x07];
	uint32_t view_entity;
	uint32_t player_slot;
	char level_name[260];
	char level_name_short[80];
	char group_name[80];
	char pad_032C[0x5C];
	uint32_t max_clients;
	char pad_0314[0x4984];
	float last_server_tick_time;
	bool in_simulation;
	char pad_4C9D[0x03];
	uint32_t oldtickcount;
	float tick_remainder;
	float frame_time;
	uint32_t last_command;
	uint32_t choked_commands;
	uint32_t last_command_ack;
	uint32_t command_ack;
	uint32_t sound_sequence;
	char pad_4CC0[0x50];
	vector_t viewangles;
	char pad_4D14[0xD0];
};