#pragma once

#include <cstdint>

class c_hook {
	uintptr_t target_;
	uintptr_t detour_;
	uintptr_t original_;

public:
	c_hook() = default;
	c_hook(uintptr_t target, uintptr_t detour);

	void enable();
	void disable();

	template <typename func_t>
	constexpr func_t get_original() const {
		return (func_t)original_;
	}
};