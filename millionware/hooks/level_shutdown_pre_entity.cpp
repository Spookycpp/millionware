#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../utils/render.hpp"

void __fastcall hooks::level_shutdown_pre_entity_hook(uintptr_t ecx, uintptr_t edx) {
	const auto _ = std::lock_guard(hooks::level_shutdown_pre_entity.call_mutex);

	cheat::local_player = nullptr;

	reinterpret_cast<decltype(&level_shutdown_pre_entity_hook)>(hooks::level_shutdown_pre_entity.original)(ecx, edx);
}
