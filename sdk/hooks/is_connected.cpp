#include "../core/hooks/hooks.h"
#include "../core/settings/settings.h"
#include "../core/patterns/patterns.h"
bool __fastcall hooks::is_connected( c_engine_client *ecx, uintptr_t edx )
{
	//if (settings.misc.unlock_inventory && reinterpret_cast<uintptr_t> (_ReturnAddress()) == patterns::inventory_unlocker)
	//	return false;

	return is_connected_original( ecx, edx );
}
