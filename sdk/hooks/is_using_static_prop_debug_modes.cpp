#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/settings/settings.h"

bool __stdcall hooks::is_using_static_prop_debug_modes() {
	return settings.visuals.world.nightmode;
}