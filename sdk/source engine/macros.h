#pragma once

#include <cstdint>

#include "../core/netvars/netvars.h"
#include "../engine/hash/hash.h"

template <typename T>
constexpr T get_vfunc(void *class_base, int index)
{
	return (*(T **) class_base)[index];
}

template <typename T, typename A>
constexpr T get_cfunc(A addr)
{
	return (T) addr;
}

#define MEMBER_FUNC_ARGS(...) (this, __VA_ARGS__); }
#define DECLARE_VFUNC(i, f, s) auto f { return get_vfunc<s>(this, i) MEMBER_FUNC_ARGS
#define DECLARE_CFUNC(f, s, a) auto f { return get_cfunc<s>(a) MEMBER_FUNC_ARGS
#define DECLARE_OFFSET(f, o) auto f { return o; }
#define DECLARE_NETVAR(t, n, e, p) t &get_##n() const { const static auto o = netvars::get(CRC_CT(e ":" p)); return *(t *) ((uintptr_t) this + o); }
#define DECLARE_NETVAR_OFFSET(t, n, e, p, a) t &get_##n() const { const static auto o = netvars::get(CRC_CT(e ":" p)); return *(t *) ((uintptr_t) this + o + a); }
#define DECLARE_NETVAR_PTR(t, n, e, p) t *get_##n() const { const static auto o = netvars::get(CRC_CT(e ":" p)); return (t *) ((uintptr_t) this + o); }
#define DECLARE_NETVAR_OFFSET_PTR(t, n, e, p, a) t *get_##n() const { const static auto o = netvars::get(CRC_CT(e ":" p)); return (t *) ((uintptr_t) this + o + a); }
