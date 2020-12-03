#pragma once

#include <cstdint>

template <typename T>
class c_valve_memory {
	uintptr_t memory_;

	int allocation_size_;
	int grow_size_;

public:
	inline uintptr_t allocation_base() const {
		return memory_;
	}

	inline int allocation_size() const {
		return allocation_size_;
	}

	inline int grow_size() const {
		return grow_size_;
	}

	inline bool is_externally_allocated() const {
		return grow_size_ < 0;
	}

	inline T& operator [](int index) {
		return reinterpret_cast<T*>(memory_)[index];
	}
};
