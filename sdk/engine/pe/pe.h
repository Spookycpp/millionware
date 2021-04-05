#pragma once

#include <cstdint>
#include <string_view>

using IMAGE_DOS_HEADER = struct _IMAGE_DOS_HEADER;
using IMAGE_NT_HEADERS = struct _IMAGE_NT_HEADERS;

namespace pe
{
	bool init();

	uint32_t get_module(std::string_view module_name);
	uint32_t get_export(std::string_view module_name, std::string_view export_name);

	IMAGE_DOS_HEADER *get_dos_hdr(std::string_view module_name);
	IMAGE_NT_HEADERS *get_nt_hdrs(std::string_view module_name);
}
