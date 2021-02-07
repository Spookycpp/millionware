#include "../core/hooks.hpp"
#include "../utils/util.hpp"

#include "../features/assistance/legitbot.hpp"

void __fastcall hooks::override_mouse_input_hook(uintptr_t ecx, uintptr_t edx, float* x, float* y) {

	if (util::run_frame() && cheat::local_player->is_alive()) {
		features::aimbot::on_override_mouse_input(x, y);
	}

	hooks::override_mouse_input.get_original<decltype(&override_mouse_input_hook)>()(ecx, edx, x, y);
}