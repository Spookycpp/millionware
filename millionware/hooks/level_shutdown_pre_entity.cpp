#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"

void __fastcall hooks::level_shutdown_pre_entity_hook(uintptr_t ecx, uintptr_t edx) {
	cheat::local_player = nullptr;

	hooks::level_shutdown_pre_entity.get_original<decltype(&level_shutdown_pre_entity_hook)>()(ecx, edx);
}