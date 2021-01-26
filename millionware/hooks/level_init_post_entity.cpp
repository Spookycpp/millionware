#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"

void __fastcall hooks::level_init_post_entity_hook(uintptr_t ecx, uintptr_t edx) {
	hooks::level_init_post_entity.get_original<decltype(&level_init_post_entity_hook)>()(ecx, edx);

	cheat::local_player = interfaces::entity_list->get_by_index(interfaces::engine_client->get_local_player())->as_player();
}