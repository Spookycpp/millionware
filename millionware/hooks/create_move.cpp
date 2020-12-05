#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../features/misc/misc.hpp"

bool __fastcall hooks::create_move_hook(uintptr_t ecx, uintptr_t edx, float frame_time, user_cmd_t* user_cmd) {
	const auto _ = std::lock_guard(hooks::create_move.call_mutex);

	features::misc::bunny_hop(user_cmd);
	features::misc::infinite_duck(user_cmd);
	features::misc::auto_strafer(user_cmd);

	reinterpret_cast<decltype(&create_move_hook)>(hooks::create_move.original)(ecx, edx, frame_time, user_cmd);

	return false;
}
