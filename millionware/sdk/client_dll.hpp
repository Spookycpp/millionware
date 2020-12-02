#pragma once

#include "client_class.hpp"
#include "macros.hpp"

class c_base_client_dll {
public:
  VIRTUAL_METHOD(client_class_t*, get_all_classes, 8, ());
  VIRTUAL_METHOD(bool, dispatch_user_msg, 38, (int msg_type, int unk0, int unk1, uint8_t* msg_data), msg_type, unk0, unk1, msg_data);
};

class c_client_mode {};
