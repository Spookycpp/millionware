#pragma once

#include <cstdint>

template <typename T>
struct linked_entry_t {
    T *next;
};

struct interface_entry_t : linked_entry_t<interface_entry_t> {
    char module_name[48];
    char interface_name[32];

    uint32_t hash = 0;
    uint32_t address = 0;
};

struct pattern_entry_t : linked_entry_t<pattern_entry_t> {
    char module_name[48];
    char pattern[128];

    uint32_t hash = 0;
    uint32_t address = 0;
};

struct netvar_table_entry_t : linked_entry_t<netvar_table_entry_t> {
    char table_name[32];

    uint32_t hash = 0;
    uint32_t offset = 0;
};

// extern __declspec(dllexport) interface_entry_t *g_interfaces;
// extern __declspec(dllexport) pattern_entry_t *g_patterns;
// extern __declspec(dllexport) netvar_table_entry_t *g_netvar_tables;

namespace exports {
    uint32_t get_interface(uint32_t hash);
    uint32_t get_pattern(uint32_t hash);
} // namespace exports
