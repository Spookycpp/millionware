#include <intrin.h>

#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../core/config.hpp"

bool __fastcall hooks::is_playing_demo_hook(uintptr_t ecx, uintptr_t edx) {
	const auto _ = std::lock_guard(hooks::is_playing_demo.call_mutex);

	return reinterpret_cast<decltype(&is_playing_demo_hook)>(hooks::is_playing_demo.original)(ecx, edx);

	// const auto return_address = reinterpret_cast<uintptr_t>(_ReturnAddress());
	// const auto stack_frame = reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) - sizeof uintptr_t;

	// if (return_address == patterns::demo_or_hltv && *reinterpret_cast<uintptr_t*>(stack_frame + 8) == patterns::money_reveal)
	//     return true;

	// return reinterpret_cast<decltype(&is_playing_demo_hook)>(hooks::is_playing_demo.original)(ecx, edx);
}
