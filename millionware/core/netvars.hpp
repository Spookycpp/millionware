#pragma once

#include <memory>
#include <unordered_map>

namespace detail {

  using netvar_tree_t = std::unordered_map<uint32_t, std::shared_ptr<struct netvar_node_t>>;

  struct netvar_node_t {
    netvar_tree_t nodes;

    int offset;
    int recv_type;

    netvar_node_t(int offset, int recv_type)
      : offset(offset), recv_type(recv_type) {}
  };

  inline int get_offset_recursive(netvar_tree_t& map, int offset, uint32_t hash) {
    return offset + map.at(hash)->offset;
  }

  template <typename... Args>
  int get_offset_recursive(netvar_tree_t& map, int offset, uint32_t hash, Args... args) {
    const auto& node = map.at(hash);

    return get_offset_recursive(node->nodes, offset + node->offset, args...);
  }

  const netvar_tree_t& get_nodes();

}

namespace netvars {

  void initialize();

  template <typename... Args>
  int get_offset(const uint32_t hash, Args... args) {
    const auto& node = detail::get_nodes().at(hash);

    return detail::get_offset_recursive(node->nodes, node->offset, args...);
  }

}
