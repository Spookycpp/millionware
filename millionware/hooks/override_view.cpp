#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../features/miscellaneous/miscellaneous.hpp"

void __fastcall hooks::override_view_hook(uintptr_t ecx, uintptr_t edx, view_setup_t* view_setup) {

	if (view_setup) {
		cheat::player_fov = view_setup->fov;
		features::miscellaneous::override_fov(view_setup);
	}

	hooks::override_view.get_original<decltype(&override_view_hook)>()(ecx, edx, view_setup);
}