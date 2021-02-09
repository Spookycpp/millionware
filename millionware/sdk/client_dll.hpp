#pragma once

#include "client_class.hpp"
#include "macros.hpp"
#include "vector.hpp"

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

class c_base_client_dll {
public:
	VIRTUAL_METHOD(client_class_t*, get_all_classes, 8, ());
	VIRTUAL_METHOD(bool, dispatch_user_msg, 38, (int msg_type, int unk0, int unk1, uint8_t* msg_data), msg_type, unk0, unk1, msg_data);
};

class c_client_mode {};

struct view_setup_t {
	char padding0_[16];
	int x;
	int x_old;
	int y;
	int y_old;
	int width;
	int width_old;
	int height;
	int height_old;
	char padding1_[128];
	float fov;
	float fov_viewmodel;
	vector3_t origin;
	vector3_t angles;
	float znear;
	float zfar;
	float znear_viewmodel;
	float zfar_viewmodel;
	float aspect_ratio;
	float near_blur_depth;
	float near_focus_depth;
	float far_focus_depth;
	float far_blur_depth;
	float near_blur_radius;
	float far_blur_radius;
	float dof_quality;
	int motion_blur_mode;
	char padding2_[68];
	int edge_blur;
};

struct c_net_channel {
	char padding0_[20];
	bool processing_messages; //0x0014
	bool should_delete; //0x0015
	char padding1_[2];
	int out_sequence_num; //0x0018 last send outgoing sequence number
	int in_sequence_num; //0x001C last received incoming sequnec number
	int out_seq_ack; //0x0020 last received acknowledge outgoing sequnce number
	int out_reliable_state; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int in_reliable_state; //0x0028 state of incoming reliable data
	int choked_packets; //0x002C number of choked packets
	float clear_time;
	float time_out;
	char name[32];

	char padding2_[1044];

	//bool transmit(bool only_reliable) {
	//	return util::get_method(this, 47).as< bool(__thiscall*)(decltype(this), bool) >()(this, only_reliable);
	//}
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
	vector3_t viewangles;
	char pad_4D14[0xD0];
};