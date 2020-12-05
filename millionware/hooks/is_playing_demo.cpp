#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../core/config.hpp"

#include <intrin.h>

bool __fastcall hooks::is_playing_demo_hook(uintptr_t ecx, uintptr_t edx) {
	const auto _ = std::lock_guard(hooks::is_playing_demo.call_mutex);

	if (!ecx)
		return reinterpret_cast<decltype(&is_playing_demo_hook)>(hooks::is_playing_demo.original)(ecx, edx);

	if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
		return reinterpret_cast<decltype(&is_playing_demo_hook)>(hooks::is_playing_demo.original)(ecx, edx);

	if (config::get<bool>(FNV_CT("misc.other.reveal_money"))) {
		// @todo: pls sig these addresses or smth this is just insane
		if (*static_cast<uintptr_t*>(_ReturnAddress()) == 0x0975C084 && **(static_cast<uintptr_t**>(_AddressOfReturnAddress()) + 4) == 0x0C75C084)
			return true;
	}

	reinterpret_cast<decltype(&is_playing_demo_hook)>(hooks::is_playing_demo.original)(ecx, edx);

	return false;
}
