#pragma once

#include "recv_props.h"
#include <cstdint>

namespace netvars {
    void init();

    send_prop_type get_type_from_netvar(uint32_t hash);

    uint32_t get(uint32_t hash);
} // namespace netvars
