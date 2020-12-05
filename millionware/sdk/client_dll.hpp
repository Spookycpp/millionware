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