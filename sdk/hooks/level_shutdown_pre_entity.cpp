#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"

void __fastcall hooks::level_shutdown_pre_entity(c_base_client_dll *ecx, uintptr_t edx)
{
	cheat::local_player = nullptr;

	level_shutdown_pre_entity_original(ecx, edx);
}
