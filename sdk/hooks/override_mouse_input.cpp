#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/util/util.h"
#include "../core/settings/settings.h"

#include "../engine/input/input.h"

#include "../features/assistance/legitbot.h"
#include "../features/movement/movement.h"

void __fastcall hooks::override_mouse_input(c_client_mode *ecx, uintptr_t edx, float *x, float *y)
{
	if (cheat::local_player->is_alive())
		features::legitbot::on_override_mouse_input(x, y);

	return override_mouse_input_original(ecx, edx, x, y);

	if (input::is_key_down(settings.miscellaneous.movement.edge_bug_assist_hotkey) && features::movement::edgebug_container::predicted_successful)
		*x *= (1.f - (0.1 * settings.miscellaneous.movement.edgebug_rage_amount));
}
