#include "../utils/hash/hash.hpp"
#include "../utils/xorstr/xorstr.hpp"
#include "interfaces.hpp"
#include "netvars.hpp"

static detail::netvar_tree_t nodes = {};

void populate_nodes(recv_table_t* recv_table, detail::netvar_tree_t& map) {
	for (auto i = 0; i < recv_table->prop_count; i++) {
		const auto prop = &recv_table->props[i];

		if (FNV(prop->var_name) == FNV_CT("baseclass") || prop->var_name[0] == '0' || prop->var_name[0] == '1' || prop->var_name[0] == '2')
			continue;

		const auto prop_info = std::make_shared<detail::netvar_node_t>(prop->offset, static_cast<int>(prop->recv_type));

		if (prop->recv_type == SEND_PROP_TYPE_DATA_TABLE)
			populate_nodes(prop->data_table, prop->offset == 0 ? map : prop_info->nodes);

		map.emplace(FNV(prop->var_name), prop_info);
	}
}

const detail::netvar_tree_t& detail::get_nodes() {
	return nodes;
}

void netvars::initialize() {
	for (auto client_class = interfaces::client->get_all_classes(); client_class != nullptr; client_class = client_class->next) {
		const auto class_info = std::make_shared<detail::netvar_node_t>(0, 0);

		const auto recv_table = client_class->recv_table;

		populate_nodes(recv_table, class_info->nodes);

		nodes.emplace(FNV(recv_table->net_table_name), class_info);
	}
}