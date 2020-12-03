#pragma once

#include "recv_props.hpp"
#include "typedefs.hpp"

struct client_class_t {
	create_client_class_fn create_fn;
	create_event_fn create_event_fn;
	const char* network_name;
	recv_table_t* recv_table;
	client_class_t* next;
	int class_id;
};
