#pragma once

#include <cstdint>
#include <fmt/format.h>

#include "../core/netvars.hpp"
#include "../utils/error/error.hpp"
#include "../utils/xorstr/xorstr.hpp"

template <typename T, const size_t Index, typename... Args>
constexpr T call_vfunc(uintptr_t class_base, Args... args) {
	using vfunc_fn = T(__thiscall*)(uintptr_t, Args...);

	return reinterpret_cast<vfunc_fn**>(class_base)[0][Index](class_base, args...);
}

static void report_faulty_netvar(uint32_t hash) {
	utils::report_error(fmt::format(STR_ENC("Couldn't find a netvar: %08X"), hash));
}

#define VIRTUAL_METHOD(return_type, name, index, args, ...)						\
	inline return_type name args {												\
		return call_vfunc<return_type, index>((uintptr_t) this, __VA_ARGS__);	\
	}

#define NETVAR_DEFINITION(type, name, ...)								\
	inline type& name() {												\
		static const auto offset = netvars::get_offset(__VA_ARGS__);	\
		if (offset == 0)												\
			report_faulty_netvar(FNV_CT(#name));						\
		return *(type*) (((uintptr_t) this) + offset);					\
	}

#define NETVAR_PTR_DEFINITION(type, name, ...)							\
	inline type* name() {												\
		static const auto offset = netvars::get_offset(__VA_ARGS__);	\
		if (offset == 0)												\
			report_faulty_netvar(FNV_CT(#name));						\
		return (type*) (((uintptr_t) this) + offset);					\
	}

#define NETVAR_DEFINITION_OFFSET(type, name, additional_offset, ...)		\
	inline type& name() {													\
		static const auto offset = netvars::get_offset(__VA_ARGS__);		\
		if (offset == 0)													\
			report_faulty_netvar(FNV_CT(#name));							\
		return *(type*) (((uintptr_t) this) + offset + additional_offset);	\
	}

#define NETVAR_PTR_DEFINITION_OFFSET(type, name, additional_offset, ...)	\
	inline type* name() {													\
		static const auto offset = netvars::get_offset(__VA_ARGS__);		\
		if (offset == 0)													\
			report_faulty_netvar(FNV_CT(#name));							\
		return (type*) (((uintptr_t) this) + offset + additional_offset);	\
	}