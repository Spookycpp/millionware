#pragma once

#include <cstdint>

#include "../core/netvars.hpp"
#include "../utils/error.hpp"

template <typename T, const size_t Index, typename... Args>
constexpr T call_vfunc(uintptr_t class_base, Args... args) {
	using vfunc_fn = T(__thiscall*)(uintptr_t, Args...);

	return reinterpret_cast<vfunc_fn**>(class_base)[0][Index](class_base, args...);
}

#define VIRTUAL_METHOD(return_type, name, index, args, ...)                                 \
  inline return_type name args {                                                            \
    return call_vfunc<return_type, index>(reinterpret_cast<uintptr_t>(this), __VA_ARGS__);  \
  }

#define NETVAR_DEFINITION(type, name, ...)                                        \
  inline type& name() {                                                           \
    static const auto offset = netvars::get_offset(__VA_ARGS__);                  \
    if (offset == 0)                                                              \
      utils::error_and_exit(e_error_code::NETVARS, FNV_CT(#name));                \
    return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset);  \
  }

#define NETVAR_PTR_DEFINITION(type, name, ...)                                  \
  inline type* name() {                                                         \
    static const auto offset = netvars::get_offset(__VA_ARGS__);                \
    if (offset == 0)                                                            \
      utils::error_and_exit(e_error_code::NETVARS, FNV_CT(#name));              \
    return reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); \
  }

#define NETVAR_OFFSET_DEFINITION(type, name, additional_offset, ...)                                  \
  inline type& name() {                                                                               \
    static const auto offset = netvars::get_offset(__VA_ARGS__);                                      \
    if (offset == 0)                                                                                  \
      utils::error_and_exit(e_error_code::NETVARS, FNV_CT(#name));                                    \
    return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset + additional_offset);  \
  }

#define NETVAR_PTR_OFFSET_DEFINITION(type, name, additional_offset, ...)                            \
  inline type* name() {                                                                             \
    static const auto offset = netvars::get_offset(__VA_ARGS__);                                    \
    if (offset == 0)                                                                                \
      utils::error_and_exit(e_error_code::NETVARS, FNV_CT(#name));                                  \
    return reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset + additional_offset); \
  }
