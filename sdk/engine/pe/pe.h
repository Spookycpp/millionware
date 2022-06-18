#pragma once

#include <cstdint>
#include <string_view>

namespace pe {

    uint32_t get_module(std::string_view module_name);
    uint32_t get_export(std::string_view module_name, std::string_view export_name);

} // namespace pe
