#pragma once

#include "macros.h"

class c_net_channel_info {
public:
    DECLARE_VFUNC(0, get_name(), const char *(__thiscall *) (void *) )();
    DECLARE_VFUNC(1, get_address(), const char *(__thiscall *) (void *) )();
    DECLARE_VFUNC(6, is_loopback(), bool(__thiscall *)(void *))();
    DECLARE_VFUNC(7, is_timing_out(), bool(__thiscall *)(void *))();
    DECLARE_VFUNC(8, is_playback(), bool(__thiscall *)(void *))();
    DECLARE_VFUNC(9, get_latency(int flow), float(__thiscall *)(void *, int))(flow);
    DECLARE_VFUNC(10, get_average_latency(int flow), float(__thiscall *)(void *, int))(flow);
};