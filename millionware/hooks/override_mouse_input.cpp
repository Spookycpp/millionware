#include "../core/hooks.hpp"

void __fastcall hooks::override_mouse_input_hook(uintptr_t ecx, uintptr_t edx, float* x, float* y) {
	hooks::override_mouse_input.get_original<decltype(&override_mouse_input_hook)>()(ecx, edx, x, y);
}