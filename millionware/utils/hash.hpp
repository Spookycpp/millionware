#pragma once

#include <cstdint>

namespace hash
{
  constexpr static uint32_t FNV_BASIS = 0x811C9DC5;
  constexpr static uint32_t FNV_PRIME = 0x01000193;

  constexpr uint32_t fnv(const char* const string, const uint32_t hash = FNV_BASIS) {
    return string[0] == 0 ? hash : fnv(&string[1], (hash ^ string[1]) * FNV_PRIME);
  }

#define FORCE_CT(value) (static_cast<decltype(value)>(hash::ct_wrapper<decltype(value), value>::VALUE))

  template<typename T, T Value>
  struct ct_wrapper
  {
    enum
    {
      VALUE = Value
    };
  };
}
