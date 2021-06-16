#include <unordered_map>
#include <vector>
#include <windows.h>
#include <winternl.h>

#include "../hash/hash.h"
#include "../logging/logging.h"
#include "../security/xorstr.h"
#include "pe.h"

inline uint32_t rel32(uint32_t call_address) {
    const auto rel_address = *(uint32_t *) (call_address + 1);
    const auto abs_address = call_address + 5 + rel_address;

    return abs_address;
}

inline uint32_t get_module_handle(wchar_t *mod_name) {
    struct _LDR_DATA_TABLE_ENTRY_ { // nice undocumented struct nn
        PVOID Reserved1[2];
        LIST_ENTRY InMemoryOrderLinks;
        PVOID Reserved2[2];
        PVOID DllBase;
        PVOID Reserved3[2];
        UNICODE_STRING FullDllName;
        UNICODE_STRING BaseDllName;
    };

    auto peb = reinterpret_cast<PEB *>(reinterpret_cast<TEB *>(__readfsdword(0x18))->ProcessEnvironmentBlock);

    auto module_list = &peb->Ldr->InMemoryOrderModuleList;

    for (auto i = module_list->Flink; i != module_list; i = i->Flink) {
        auto entry = CONTAINING_RECORD(i, _LDR_DATA_TABLE_ENTRY_, InMemoryOrderLinks);
        if (!entry)
            continue;

        wchar_t name_copy[256] = {0};

        wcscpy(name_copy, entry->BaseDllName.Buffer);

        if (_wcsicmp(name_copy, mod_name) == 0)
            return (uint32_t) entry->DllBase;
    }

    return 0ul;
}

inline uint32_t get_module_handle(const char *module_name) {
    wchar_t wide_module_name[128] = {0};
    uint32_t converted_chars = 0;
    mbstowcs_s(&converted_chars, wide_module_name, module_name, 128);

    return get_module_handle(wide_module_name);
}

inline char *find_utf8(const char *module, const char *string) {
    uint32_t module_start = (uint32_t) get_module_handle(module);
    const auto dos_headers = (IMAGE_DOS_HEADER *) module_start;
    const auto nt_headers = (IMAGE_NT_HEADERS *) (module_start + dos_headers->e_lfanew);

    uint32_t module_end = module_start + nt_headers->OptionalHeader.SizeOfImage;

    for (uint32_t current = module_start; current < module_end; current++) {
        if (strcmp((const char *) current, string) == 0)
            return (char *) current;
    }

    return nullptr;
}

inline uint32_t get_proc_address(const char *module_name, const char *function_name) {
    auto module_handle = get_module_handle(module_name);
    auto raw = (uint8_t *) module_handle;

    const auto dos = (IMAGE_DOS_HEADER *) raw;
    const auto nt64 = (IMAGE_NT_HEADERS64 *) (raw + dos->e_lfanew);
    const auto nt32 = (IMAGE_NT_HEADERS32 *) (raw + dos->e_lfanew);

    IMAGE_EXPORT_DIRECTORY *export_dir = nullptr;
    uint32_t export_size = 0;

    if (nt32->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) { // men this is 32
        auto directory = nt32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        export_dir = (IMAGE_EXPORT_DIRECTORY *) (raw + directory.VirtualAddress);
        export_size = directory.Size;
    }
    else { // men this is 64
        auto directory = nt64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        export_dir = (IMAGE_EXPORT_DIRECTORY *) (raw + directory.VirtualAddress);
        export_size = directory.Size;
    }

    const auto functions = (uint32_t *) (raw + export_dir->AddressOfFunctions);
    const auto names = (uint32_t *) (raw + export_dir->AddressOfNames);
    const auto ordinals = (uint16_t *) (raw + export_dir->AddressOfNameOrdinals);

    const auto name_count = export_dir->NumberOfNames;

    uint32_t procedure = 0;

    for (uint32_t i = 0; i < name_count; i++) {
        const auto name = (const char *) (raw + names[i]);
        if (strcmp(name, function_name) == 0) {
            procedure = functions[ordinals[i]];
            break;
        }
    }

    auto function_address = procedure + (uint32_t) raw;
    // printf("0x%lx\n", function_address);

    if (function_address >= (uint32_t) export_dir && function_address < (uint32_t) export_dir + export_size) { // forwarded export
        char forwarded_module_name[128];
        memcpy(forwarded_module_name, (void *) (function_address), 128);
        char *forwarded_function_name = strchr(forwarded_module_name, '.');

        *forwarded_function_name++ = 0;

        return get_proc_address(forwarded_module_name, forwarded_function_name);
    }

    return procedure + module_handle;
}

uint32_t pe::get_module(std::string_view module_name) {
    return get_module_handle(module_name.data());
}

uint32_t pe::get_export(std::string_view module_name, std::string_view export_name) {
    return get_proc_address(module_name.data(), export_name.data());
}
