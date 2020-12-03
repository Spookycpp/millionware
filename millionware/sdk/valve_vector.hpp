#pragma once

#include "valve_memory.hpp"

template <typename T, class Alloc = c_valve_memory<T>>
class c_valve_vector {
	Alloc allocator_;
	int element_count_;
	T* elements_;

public:
	inline T& begin() {
		return reinterpret_cast<T*>(allocator_.allocation_base())[0];
	}

	inline const T& begin() const {
		return reinterpret_cast<const T*>(allocator_.allocation_base())[0];
	}

	inline T& end() {
		return reinterpret_cast<T*>(allocator_.allocation_base())[element_count_];
	}

	inline const T& end() const {
		return reinterpret_cast<const T*>(allocator_.allocation_base())[element_count_];
	}

	inline T& operator[](int i) {
		return allocator_[i];
	}

	inline const T& operator[](int i) const {
		return allocator_[i];
	}

	int element_count() const {
		return element_count_;
	}

	void element_count(int value) {
		element_count_ = value;
	}

	inline T& element(int i) {
		return allocator_[i];
	}

	inline const T& element(int i) const {
		return allocator_[i];
	}
};
