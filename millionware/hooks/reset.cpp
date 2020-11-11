#include <Windows.h>

#include <cstdint>
#include <d3dx9.h>

#include "../core/hooks.hpp"

long __fastcall reset_hook(const uintptr_t ecx, const uintptr_t edx, const D3DPRESENT_PARAMETERS* presentation_parameters) {
	const auto _ = std::lock_guard<std::mutex>(hooks::reset.call_mutex);

	return reinterpret_cast<decltype(&reset_hook)>(hooks::reset.original)(ecx, edx, presentation_parameters);
}
