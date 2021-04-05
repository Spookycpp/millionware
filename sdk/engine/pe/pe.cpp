#include <unordered_map>
#include <vector>
#include <windows.h>
#include <winternl.h>

#include "../hash/hash.h"
#include "../logging/logging.h"
#include "../security/xorstr.h"
#include "pe.h"

struct module_info_t
{
	uint32_t address;

	std::unordered_map< uint32_t, uint32_t > exports;
};

static std::unordered_map< uint32_t, module_info_t > modules;

static void dump_module(LDR_DATA_TABLE_ENTRY *module_entry)
{
	const auto base_dll_name = (UNICODE_STRING *) &module_entry->Reserved4[0];
	const auto module_name_wide = std::wstring(base_dll_name->Buffer, base_dll_name->Length >> 1);
	const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &module_name_wide[0], (int) module_name_wide.size(), nullptr, 0, nullptr, nullptr);

	auto module_name = std::string(size_needed, '\x00');

	WideCharToMultiByte(CP_UTF8, 0, &module_name_wide[0], (int) module_name_wide.size(), &module_name[0], size_needed, NULL, NULL);

	auto &module_info = modules[CRC(module_name.data())] =
	{
		(uint32_t) module_entry->DllBase,
	};

	const auto dos_hdr = (IMAGE_DOS_HEADER *) module_info.address;
	const auto nt_hdrs = (IMAGE_NT_HEADERS32 *) (module_info.address + dos_hdr->e_lfanew);
	const auto export_dir = nt_hdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

	const auto exports_data = (IMAGE_EXPORT_DIRECTORY *) (module_info.address + export_dir.VirtualAddress);
	const auto exports_count = export_dir.Size;

	const auto export_functions = (uint32_t *) (module_info.address + exports_data->AddressOfFunctions);
	const auto export_names = (uint32_t *) (module_info.address + exports_data->AddressOfNames);
	const auto export_ordinals = (uint16_t *) (module_info.address + exports_data->AddressOfNameOrdinals);
	const auto export_name_count = exports_data->NumberOfNames;

	for (auto i = 0u; i < export_name_count; i++)
	{
		const auto export_name = (const char *) (module_info.address + export_names[i]);

		module_info.exports[CRC(export_name)] = (uint32_t) export_functions[export_ordinals[i]];
	}

	logging::trace(XORSTR("found module '{}' with {} exports at address 0x{:08x}."), module_name.data(), module_info.exports.size(), module_info.address);
}

static bool try_dump_module(std::string_view name)
{
	const auto peb = (_PEB *) __readfsdword(0x30);

	if (!peb)
	{
		logging::error(XORSTR("failed to retrieve the process environment block"));

		return false;
	}

	const auto list = &peb->Ldr->InMemoryOrderModuleList;

	for (auto it = list->Flink; it != list; it = it->Flink)
	{
		const auto entry = CONTAINING_RECORD(it, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

		if (!entry)
			continue;

		const auto base_dll_name = (UNICODE_STRING *) &entry->Reserved4[0];
		const auto module_name_wide = std::wstring(base_dll_name->Buffer, base_dll_name->Length >> 1);
		const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &module_name_wide[0], (int) module_name_wide.size(), nullptr, 0, nullptr, nullptr);

		auto module_name = std::string(size_needed, '\x00');

		WideCharToMultiByte(CP_UTF8, 0, &module_name_wide[0], (int) module_name_wide.size(), &module_name[0], size_needed, NULL, NULL);

		if (module_name.compare(name) != 0)
			continue;

		dump_module(entry);

		return true;
	}

	return false;
}

static module_info_t *get_module_info(std::string_view name, bool try_dump = true)
{
	const auto module_hash = CRC(name.data());

	if (modules.find(module_hash) == modules.end())
	{
		if (try_dump && try_dump_module(name))
			return get_module_info(name, false);

		logging::error(XORSTR("couldn't find module '{}'"), name.data());

		return 0u;
	}

	return &modules[module_hash];
}

bool pe::init()
{
	modules.clear();

	const auto peb = (_PEB *) __readfsdword(0x30);

	if (!peb)
	{
		logging::error(XORSTR("failed to retrieve the process environment block"));

		return false;
	}

	const auto list = &peb->Ldr->InMemoryOrderModuleList;

	for (auto it = list->Flink; it != list; it = it->Flink)
	{
		const auto entry = CONTAINING_RECORD(it, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

		if (!entry)
			continue;

		dump_module(entry);
	}

	logging::debug(XORSTR("successfully listed all modules from memory"));

	return true;
}

uint32_t pe::get_module(std::string_view module_name)
{
	const auto module_info = get_module_info(module_name);

	if (!module_info)
		return 0u;

	return module_info->address;
}

uint32_t pe::get_export(std::string_view module_name, std::string_view export_name)
{
	const auto module_info = get_module_info(module_name);

	if (!module_info)
		return 0u;

	const auto export_hash = CRC(export_name.data());

	if (module_info->exports.find(export_hash) == module_info->exports.end())
	{
		logging::error(XORSTR("couldn't find module '{}'"), export_name.data(), module_name.data());

		return 0u;
	}

	return module_info->address + module_info->exports.at(export_hash);
}

IMAGE_DOS_HEADER *pe::get_dos_hdr(std::string_view module_name)
{
	const auto module_info = get_module_info(module_name);

	if (module_info != nullptr)
		return (IMAGE_DOS_HEADER *) module_info->address;

	return 0u;
}

IMAGE_NT_HEADERS *pe::get_nt_hdrs(std::string_view module_name)
{
	const auto module_info = get_module_info(module_name);

	if (module_info != nullptr)
	{
		const auto dos_hdr = (IMAGE_DOS_HEADER *) module_info->address;

		return (IMAGE_NT_HEADERS *) (module_info->address + dos_hdr->e_lfanew);
	}

	return 0u;
}
