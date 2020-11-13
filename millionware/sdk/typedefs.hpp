#pragma once

#include <cstdint>

using create_client_class_fn = uintptr_t(*)(int, int);
using create_event_fn = uintptr_t(*)();

using recv_var_proxy_fn = void (*)(uintptr_t, uintptr_t, uintptr_t);
using array_length_recv_proxy_fn = void (*)(uintptr_t, int, int);
using data_table_recv_var_proxy_fn = void (*)(struct recv_prop_t*, uintptr_t, uintptr_t, int);

using vgui_font_t = uint32_t;
using vgui_texture_t = uint32_t;
