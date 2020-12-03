#pragma once

#include "convar.hpp"
#include "macros.hpp"

class c_convar_system {
public:
	VIRTUAL_METHOD(c_convar*, find, 14, (std::string_view name), name.data());
};
