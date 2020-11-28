#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"

bool __fastcall hooks::create_move_hook(uintptr_t ecx, uintptr_t edx, float frame_time, user_cmd_t* user_cmd) {
	const auto _ = std::lock_guard(hooks::create_move.call_mutex);

	if (user_cmd->buttons & BUTTON_IN_JUMP && !(cheat::local_player->flags() & ENTITY_FLAG_ONGROUND))
		user_cmd->buttons &= ~BUTTON_IN_JUMP;

	user_cmd->buttons |= BUTTON_IN_BULLRUSH;

	reinterpret_cast<decltype(&create_move_hook)>(hooks::create_move.original)(ecx, edx, frame_time, user_cmd);

	return false;
}
