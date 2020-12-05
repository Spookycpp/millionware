#include "../core/hooks.hpp"

void __fastcall hooks::override_mouse_input_hook(uintptr_t ecx, uintptr_t edx, float* x, float* y) {
	const auto _ = std::lock_guard(hooks::override_mouse_input.call_mutex);

	reinterpret_cast<decltype(&override_mouse_input_hook)>(hooks::override_mouse_input.original)(ecx, edx, x, y);
}