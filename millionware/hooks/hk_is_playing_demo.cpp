#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../core/config.hpp"

#include <intrin.h>

bool __fastcall hooks::playing_demo_hook(uintptr_t* ecx, uintptr_t* edx) {
	const auto _ = std::lock_guard(hooks::hk_is_playing_demo.call_mutex);

	if (!ecx)
		return reinterpret_cast<decltype(&playing_demo_hook)>(hooks::create_move.original)(ecx, edx);

	if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
		return reinterpret_cast<decltype(&playing_demo_hook)>(hooks::create_move.original)(ecx, edx);

	if (config::get<bool>(FNV_CT("misc.other.reveal_money"))) {
		if (*static_cast<uintptr_t*>(_ReturnAddress()) == 0x0975C084 && **reinterpret_cast<uintptr_t**>(uintptr_t(_AddressOfReturnAddress()) + 4) == 0x0C75C084)
			return true;
	}

	reinterpret_cast<decltype(&playing_demo_hook)>(hooks::hk_is_playing_demo.original)(ecx, edx);

	return false;
}
