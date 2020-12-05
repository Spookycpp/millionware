#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../features/misc/misc.hpp"

void __fastcall hooks::override_view_hook(uintptr_t ecx, uintptr_t edx, view_setup_t* view_setup) {
	if (view_setup) {
		cheat::player_fov = view_setup->fov;
		features::misc::override_fov(view_setup);
	}

	reinterpret_cast<decltype(&override_view_hook)>(hooks::override_view.original)(ecx, edx, view_setup);
}