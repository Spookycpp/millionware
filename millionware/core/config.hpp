#pragma once

#include <cstdint>
#include <string_view>
#include <unordered_map>

namespace config
{
  namespace detail
  {
    struct config_item_t
    {
      uint8_t data[64] = { 0 };
    };

    inline std::unordered_map<uint32_t, config_item_t> items = {};
  }

  void initialize();

  void save_to(std::string_view name);
  void load_from(std::string_view name);

  template <typename T>
  constexpr T& get(uint32_t item) {
    static_assert(sizeof(T) <= sizeof detail::config_item_t::data, "type is too big");

    if (detail::items.find(item) == detail::items.end())
      detail::items[item] = detail::config_item_t{};

    return *reinterpret_cast<T*>(detail::items[item].data);
  }
};
