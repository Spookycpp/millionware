#pragma once

#include <cstdint>

namespace utils
{
  constexpr static uint32_t FNV_BASIS = 0x811C9DC5;
  constexpr static uint32_t FNV_PRIME = 0x01000193;

  constexpr uint32_t hash_fnv(const char* string, uint32_t hash = FNV_BASIS) {
    return string[0] == 0 ? hash : hash_fnv(&string[1], (hash ^ string[1]) * FNV_PRIME);
  }

  template<typename T, T Value>
  struct ct_wrapper
  {
    enum
    {
      VALUE = Value
    };
  };
}

#define FORCE_CT(value) (static_cast<decltype(value)>(utils::ct_wrapper<decltype(value), value>::VALUE))
#define HASH_FNV(string) utils::hash_fnv(string)
#define HASH_FNV_CT(string) FORCE_CT(HASH_FNV(string))
