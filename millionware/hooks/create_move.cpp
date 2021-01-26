#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"

#include "../features/movement/movement.hpp"
#include "../features/movement/prediction/engine_prediction.hpp"
#include "../features/miscellaneous/miscellaneous.hpp"

static std::once_flag prediction_init;

bool __fastcall hooks::create_move_hook(uintptr_t ecx, uintptr_t edx, float frame_time, user_cmd_t* user_cmd) {
	std::call_once(prediction_init, engine_prediction::init);

	if (!user_cmd || !user_cmd->command_number)
		return hooks::create_move.get_original<decltype(&create_move_hook)>()(ecx, edx, frame_time, user_cmd);

	features::miscellaneous::rank_reveal();
	features::miscellaneous::auto_pistol(user_cmd);

	features::movement::pre_prediction(user_cmd);

	const auto pre_flags = cheat::local_player->flags();

	engine_prediction::start_prediction(user_cmd);
	engine_prediction::end_prediction(user_cmd);

	const auto post_flags = cheat::local_player->flags();
	features::movement::post_prediction(user_cmd, pre_flags, post_flags);

	hooks::create_move.get_original<decltype(&create_move_hook)>()(ecx, edx, frame_time, user_cmd);

	return false;
}