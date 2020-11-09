#include <Windows.h>

#include <cstdint>
#include <d3dx9.h>

#include "../core/hooks.hpp"

long __fastcall reset_hook(const uintptr_t ecx, const uintptr_t edx, const D3DPRESENT_PARAMETERS* presentation_parameters) {
	return reinterpret_cast<decltype(&reset_hook)>(hooks::original_reset)(ecx, edx, presentation_parameters);
}
