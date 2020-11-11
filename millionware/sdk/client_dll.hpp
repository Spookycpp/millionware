#pragma once

#include "client_class.hpp"
#include "macros.hpp"

class c_base_client_dll
{
public:
  VIRTUAL_METHOD(client_class_t*, get_all_classes, 8, ());
};
