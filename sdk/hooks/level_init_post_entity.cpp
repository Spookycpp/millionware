#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"

void __fastcall hooks::level_init_post_entity(c_base_client_dll* ecx, uintptr_t edx) {

	level_init_post_entity_original(ecx, edx);

	cheat::local_player = (c_player*)interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player());
}
