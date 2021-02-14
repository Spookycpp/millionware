#pragma once
#include "macros.hpp"

class c_localize {
public:
	VIRTUAL_METHOD( wchar_t*, find, 11, ( const char* name ), name );
};