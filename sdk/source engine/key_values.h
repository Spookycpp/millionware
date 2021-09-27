#pragma once

#include "macros.h"

#include "../core/patterns/patterns.h"

class c_key_values {
public:
    c_key_values *init(const char *name) {
        using ctor_t = c_key_values *(__thiscall *) (void *, const char *);

        static auto ctor = reinterpret_cast<ctor_t>(patterns::get_key_values_system());
        return ctor(this, name);
    };

    // xref '%s\\gameinfo.txt'
    bool load_from_buffer(const char *name, const char *buffer) {
        using load_t = bool(__thiscall *)(c_key_values *, const char *, const char *, void *, void *, void *);

        static auto load_from_buffer_fn = reinterpret_cast<load_t>(patterns::get_load_from_buffer());
        return load_from_buffer_fn(this, name, buffer, 0, 0, 0);
    }

    c_key_values *find_key(const char *name, bool create) {
        using findkey_t = c_key_values *(__thiscall *) (c_key_values *, const char *, bool);
        static auto find_key_fn = reinterpret_cast<findkey_t>(patterns::get_find_key());
        return find_key_fn(this, name, create);
    }

    void set_int(const char *key_name, int val) {
        auto key_int = find_key(key_name, true);

        if (key_int) {
            *reinterpret_cast<int *>(reinterpret_cast<uintptr_t>(key_int) + 0xC) = val;
            *reinterpret_cast<char *>(reinterpret_cast<uintptr_t>(key_int) + 0x10) = 2;
        }
    }

    void set_string(const char *name, const char *a2) {
        auto key = find_key(name, true);
        using setstring_t = void(__thiscall *)(void *, const char *);

        if (key) {
            static auto set_string_fn = reinterpret_cast<setstring_t>(patterns::get_set_string());
            set_string_fn(key, a2);
        }
    }
};