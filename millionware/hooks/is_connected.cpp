#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../core/config.hpp"

bool __fastcall hooks::is_connected_hook(uintptr_t ecx, uintptr_t edx) {

	if (config::get<bool>(FNV_CT("misc.other.unlock_inventory")) && reinterpret_cast<uintptr_t> (_ReturnAddress()) == patterns::inventory_unlocker)
		return false;

	return hooks::is_connected.get_original<decltype(&is_connected_hook)>()(ecx, edx);
}