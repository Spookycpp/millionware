#pragma once

#include "macros.hpp"

class c_input_system {
public:
	VIRTUAL_METHOD(void, enable_input, 11, (bool state), state);
};
