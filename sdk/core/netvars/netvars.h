#pragma once

#include <cstdint>
#include "recv_props.h"

namespace netvars
{
	void init();

	send_prop_type get_type_from_netvar(uint32_t hash);

	uint32_t get(uint32_t hash);
}
