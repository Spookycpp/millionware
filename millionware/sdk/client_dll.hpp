#pragma once

#include "client_class.hpp"
#include "macros.hpp"

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
