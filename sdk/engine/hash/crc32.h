#pragma once

#include <cstdint>
#include <cstring>

constexpr uint32_t crc_ct(const char *data, uint32_t value = 0) {
    return data[0] == '\x00' ? value : crc_ct(&data[1], value * 31 + (uint32_t) data[0]);
}

constexpr uint32_t crc_ct(const wchar_t *data, uint32_t value = 0) {
    return data[0] == L'\x0000' ? value : crc_ct(&data[1], value * 31 + (uint32_t) data[0]);
}
