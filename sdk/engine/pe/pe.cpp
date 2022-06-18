#include <unordered_map>
#include <vector>
#include <windows.h>
#include <winternl.h>

#include "../hash/hash.h"
#include "../logging/logging.h"
#include "../security/xorstr.h"
#include "pe.h"

inline uint32_t get_module_handle(const char *module_name) {
    return (uint32_t) GetModuleHandleA(module_name);
}

inline uint32_t get_proc_address(const char *module_name, const char *function_name) {
    auto module_handle = get_module_handle(module_name);

    return (uint32_t) GetProcAddress((HMODULE) module_handle, function_name);
}

uint32_t pe::get_module(std::string_view module_name) {
    return get_module_handle(module_name.data());
}

uint32_t pe::get_export(std::string_view module_name, std::string_view export_name) {
    return get_proc_address(module_name.data(), export_name.data());
}
