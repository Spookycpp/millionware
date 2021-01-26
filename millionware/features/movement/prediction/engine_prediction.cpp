#include "engine_prediction.hpp"

#include "../../../core/cheat.hpp"

inline c_move_helper* move_helper = nullptr;

void engine_prediction::init() {
	move_helper = **reinterpret_cast<c_move_helper***>((patterns::move_helper + 2));
}

void engine_prediction::start_prediction(user_cmd_t* cmd) {

	if (last_command) {

		if (last_command->has_been_predicted) {
			correct_tickbase = cheat::local_player->tick_base();
		}
		else {
			++correct_tickbase;
		}
	}

	last_command = cmd;
	old_tickbase = cheat::local_player->tick_base();

	old_curtime = interfaces::global_vars->current_time;
	old_frametime = interfaces::global_vars->frame_time;

	cheat::local_player->tick_base() = correct_tickbase;

	interfaces::global_vars->current_time = cheat::local_player->tick_base() * interfaces::global_vars->interval_per_tick;
	interfaces::global_vars->frame_time = interfaces::global_vars->interval_per_tick;

	interfaces::game_movement->start_track_prediction_errors(cheat::local_player);

	movedata_t data;
	memset(&data, 0, 184);

	move_helper->set_host(cheat::local_player);
	interfaces::prediction->setup_move(cheat::local_player, cmd, move_helper, &data);
	interfaces::game_movement->process_movement(cheat::local_player, &data);
	interfaces::prediction->finish_move(cheat::local_player, cmd, &data);
}

void engine_prediction::store(user_cmd_t* cmd) {
	cheat::unpredicted_flags = cheat::local_player->flags();
}

void engine_prediction::end_prediction(user_cmd_t* cmd) {

	interfaces::game_movement->finish_track_prediction_errors(cheat::local_player);
	move_helper->set_host(nullptr);

	cheat::local_player->tick_base() = old_tickbase;

	interfaces::global_vars->current_time = engine_prediction::old_curtime;
	interfaces::global_vars->frame_time = engine_prediction::old_frametime;
}