#pragma once

#include "crc32.h"

template <typename T, T val>
struct ct_wrapper_t {
    constexpr static const T value = val;
};

#define CRC(...) crc_ct(__VA_ARGS__)
#define CRC_CT(...) ct_wrapper_t<uint32_t, crc_ct(__VA_ARGS__)>::value
