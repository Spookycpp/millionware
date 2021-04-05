#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"

#include "../features/assistance/legitbot.h"

void __fastcall hooks::override_mouse_input(c_client_mode *ecx, uintptr_t edx, float *x, float *y)
{
	if (util::run_frame() && cheat::local_player->is_alive()) {
		features::legitbot::on_override_mouse_input(x, y);
	}

	return override_mouse_input_original(ecx, edx, x, y);
}
