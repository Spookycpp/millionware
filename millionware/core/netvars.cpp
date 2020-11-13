#include "../utils/hash.hpp"
#include "interfaces.hpp"
#include "netvars.hpp"

static netvars::detail::netvar_tree_t nodes = {};

void populate_nodes(recv_table_t* recv_table, netvars::detail::netvar_tree_t& map) {
	for (auto i = 0; i < recv_table->prop_count; i++) {
		const auto prop = &recv_table->props[i];

		const auto prop_info = std::make_shared<netvars::detail::netvar_node_t>(prop->offset, static_cast<int>(prop->recv_type));

		if (prop->recv_type == e_send_prop_type::DATA_TABLE)
			populate_nodes(prop->data_table, prop_info->nodes);

		map.emplace(HASH_FNV(prop->var_name), prop_info);
	}
}

const netvars::detail::netvar_tree_t& netvars::detail::get_nodes() {
	return nodes;
}

void netvars::initialize() {
	for (auto client_class = interfaces::client->get_all_classes(); client_class != nullptr; client_class = client_class->next) {
		const auto class_info = std::make_shared<detail::netvar_node_t>(0, 0);
		const auto recv_table = client_class->recv_table;

		populate_nodes(recv_table, class_info->nodes);

		nodes.emplace(HASH_FNV(recv_table->net_table_name), class_info);
	}
}
