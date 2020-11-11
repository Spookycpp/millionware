#pragma once

#include "macros.hpp"

class c_net_channel_info
{
public:
  VIRTUAL_METHOD(const char*, get_name, 0, ());
  VIRTUAL_METHOD(const char*, get_address, 1, ());
  VIRTUAL_METHOD(bool, is_loopback, 6, ());
  VIRTUAL_METHOD(bool, is_timing_out, 7, ());
  VIRTUAL_METHOD(bool, is_playback, 8, ());
  VIRTUAL_METHOD(float, get_latency, 9, (int flow), flow);
  VIRTUAL_METHOD(float, get_average_latency, 10, (int flow), flow);
};
