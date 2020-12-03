#pragma once

#include "base_handle.hpp"
#include "entity.hpp"
#include "macros.hpp"

class c_entity;

class c_entity_list {
public:
	VIRTUAL_METHOD(c_entity*, get_by_index, 3, (const int index), index);
	VIRTUAL_METHOD(c_entity*, get_by_handle, 4, (const base_handle_t& handle), handle.handle);
	VIRTUAL_METHOD(int, get_highest_entity_index, 6, ());
};
