#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../utils/render.hpp"

void __fastcall hooks::level_init_post_entity_hook(uintptr_t ecx, uintptr_t edx) {
	const auto _ = std::lock_guard(hooks::level_init_post_entity.call_mutex);

	reinterpret_cast<decltype(&level_init_post_entity_hook)>(hooks::level_init_post_entity.original)(ecx, edx);

	cheat::local_player = interfaces::entity_list->get_by_index(interfaces::engine_client->get_local_player())->as_player();
}
