#pragma once

#include <cstdint>

using create_client_class_fn = uintptr_t(*)(int, int);
using create_event_fn = uintptr_t(*)();

using recv_var_proxy_fn = void (*)(uintptr_t, uintptr_t, uintptr_t);
using array_length_recv_proxy_fn = void (*)(uintptr_t, int, int);
using data_table_recv_var_proxy_fn = void (*)(struct recv_prop_t*, uintptr_t, uintptr_t, int);

using model_handle_t = unsigned int;

using vgui_font_t = unsigned long;
using vgui_texture_t = unsigned long;

using fn_change_callback_t = void (*)(class c_convar*, const char*, float);
