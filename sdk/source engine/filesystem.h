#pragma once

#include "macros.h"

class c_base_file_system {
public:
    DECLARE_VFUNC(0, read(void *output, int size, void *file), int(__thiscall *)(void *, void *, int, void *))(output, size, file);
    DECLARE_VFUNC(2, open(const char *file_name, const char *options, const char *path_id = nullptr), void *(__thiscall *)(void *, const char *, const char *, const char *))(file_name, options, path_id);
    DECLARE_VFUNC(3, close(void *file), void(__thiscall *)(void *, void *))(file);
    DECLARE_VFUNC(7, size(void *file), unsigned int(__thiscall *)(void *, void *))(file);
};