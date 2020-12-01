#pragma once

#include "hash.hpp"

#include <immintrin.h>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <type_traits>

namespace detail
{
  template <size_t Size>
  __forceinline constexpr size_t buffer_size() {
    return ((Size / 16) + (Size % 16 != 0)) * 2;
  }

  template <uint32_t Seed>
  __forceinline constexpr uint32_t key_32() {
    auto value = Seed;

    for (const auto ch : __TIME__)
      value = static_cast<uint32_t>((value ^ ch) * 0x01000193ull);

    return value;
  }

  template <size_t Size>
  __forceinline constexpr uint64_t key_64() {
    constexpr auto first_part = key_32<0x811C9DC5 + Size>();
    constexpr auto second_part = key_32<first_part>();

    return (static_cast<uint64_t>(first_part) << 32) | second_part;
  }

  template <typename char_type_t, size_t Length, size_t N>
  __forceinline constexpr uint64_t load_xored_str_32(uint64_t key, size_t idx, const char_type_t(&str)[Length]) {
    constexpr auto value_size = sizeof char_type_t;
    constexpr auto idx_offset = 8 / value_size;
    
    auto value = key;

    for (auto i = 0; i < idx_offset && i + idx * idx_offset < N; i++)
      value ^= static_cast<uint64_t>(static_cast<char_type_t>(str[i + idx * idx_offset])) << ((i % idx_offset) * 8 * value_size);

    return value;
  }

  __forceinline uint64_t load_from_reg(uint64_t value) noexcept {
    return value;
  }
}

template <typename char_type_t, size_t Size, typename Keys, typename Indices>
class xor_string;

template<typename char_type_t, size_t Size, uint64_t... Keys, size_t... Indices>
class xor_string<char_type_t, Size, std::integer_sequence<uint64_t, Keys...>, std::index_sequence<Indices...>>
{
  alignas(((Size > 16) ? 32 : 16)) uint64_t storage_[sizeof...(Keys)];

public:
  __forceinline xor_string(const char_type_t(&str)[Size], std::integral_constant<size_t, Size>, std::index_sequence<Indices...>)
    : storage_{ detail::load_xored_str_32<char_type_t, Size, Size>(Keys, Indices, str)... } {}

  __forceinline constexpr size_t size() const {
    return Size - 1;
  }

  __forceinline void crypt() {
    alignas(((Size > 16) ? 32 : 16)) const uint64_t keys[] = { Keys... };

    ((Indices >= sizeof(storage_) / 32
      ? static_cast<void>(0)
      : _mm256_store_si256(reinterpret_cast<__m256i*>(storage_) + Indices, _mm256_xor_si256(_mm256_load_si256(reinterpret_cast<const __m256i*>(storage_) + Indices), _mm256_load_si256(reinterpret_cast<const __m256i*>(keys) + Indices)))), ...);

    if constexpr (sizeof(storage_) % 32 != 0)
      _mm_store_si128(reinterpret_cast<__m128i*>(storage_ + sizeof...(Keys) - 2), _mm_xor_si128(_mm_load_si128(reinterpret_cast<const __m128i*>(storage_ + sizeof...(Keys) - 2)), _mm_load_si128(reinterpret_cast<const __m128i*>(keys + sizeof...(Keys) - 2))));
  }

  __forceinline const char_type_t* get() const {
    return reinterpret_cast<const char_type_t*>(storage_);
  }

  __forceinline char_type_t* get() {
    return reinterpret_cast<char_type_t*>(storage_);
  }

  __forceinline const char_type_t* crypt_get() const {
    crypt();

    return get();
  }

  __forceinline char_type_t* crypt_get() {
    crypt();

    return get();
  }
};

template<typename char_type_t, size_t Size, size_t... Indices>
xor_string(const char_type_t(&str)[Size], std::integral_constant<size_t, Size>, std::index_sequence<Indices...>) -> xor_string<char_type_t, Size, std::integer_sequence<uint64_t, detail::key_32<Indices>()...>, std::index_sequence<Indices...>>;

#define XORSTR(str) xor_string(str, std::integral_constant<size_t, sizeof(str) / sizeof(str[0])>{}, std::make_index_sequence<detail::buffer_size<sizeof(str)>()>{}).crypt_get()
