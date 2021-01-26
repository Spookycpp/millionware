#pragma once

#include <cstdint>

namespace utils {

	constexpr static uint32_t FNV_BASIS = 0x811C9DC5;
	constexpr static uint32_t FNV_PRIME = 0x01000193;

	constexpr uint32_t hash_fnv(const char* string, uint32_t hash = FNV_BASIS) {
		return string[0] == 0 ? hash : hash_fnv(&string[1], (hash ^ string[0]) * FNV_PRIME);
	}

	constexpr uint32_t hash_fnv(const char* data, int data_size, uint32_t hash = FNV_BASIS) {
		return data_size == 0 ? hash : hash_fnv(&data[1], data_size - 1, (hash ^ data[0]) * FNV_PRIME);
	}

	constexpr uint32_t hash_fnv(const wchar_t* string, uint32_t hash = FNV_BASIS) {
		return string[0] == 0 ? hash : hash_fnv(&string[1], (hash ^ string[0]) * FNV_PRIME);
	}

	constexpr uint32_t hash_fnv(uint32_t other_hash, uint32_t hash = FNV_BASIS) {
		return hash_fnv(reinterpret_cast<const char*>(&other_hash), sizeof uint32_t, hash);
	}

	template<typename T, T Value>
	struct ct_wrapper {
		enum {
			VALUE = Value
		};
	};

}

#define FORCE_CT(value) (static_cast<decltype(value)>(utils::ct_wrapper<decltype(value), value>::VALUE))
#define FNV(...) utils::hash_fnv(__VA_ARGS__)
#define FNV_CT(...) FORCE_CT(FNV(__VA_ARGS__))
