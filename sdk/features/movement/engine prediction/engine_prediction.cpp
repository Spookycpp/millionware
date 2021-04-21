#include "engine_prediction.h"

#include "../../../core/cheat/cheat.h"

#include "../../../source engine/client_prediction.h"

#include "../movement.h"

inline c_move_helper* move_helper = nullptr;

void engine_prediction::init() {
	move_helper = **reinterpret_cast<c_move_helper***>((patterns::move_helper + 2));
}

void engine_prediction::start_prediction(c_user_cmd* user_cmd) {

	if (last_command) {

		if (last_command->has_been_predicted) {
			correct_tickbase = cheat::local_player->get_tick_base();
		}
		else {
			++correct_tickbase;
		}
	}

	last_command = user_cmd;
	old_tickbase = cheat::local_player->get_tick_base();

	old_curtime = interfaces::global_vars->current_time;
	old_frametime = interfaces::global_vars->frame_time;

	cheat::local_player->get_tick_base() = correct_tickbase;

	interfaces::global_vars->current_time = cheat::local_player->get_tick_base() * interfaces::global_vars->interval_per_tick;
	interfaces::global_vars->frame_time = interfaces::global_vars->interval_per_tick;

	interfaces::game_movement->start_track_prediction_errors(cheat::local_player);

	movedata_t data;
	memset(&data, 0, 184);

	move_helper->set_host(cheat::local_player);
	interfaces::prediction->setup_move(cheat::local_player, user_cmd, move_helper, &data);
	interfaces::game_movement->process_movement(cheat::local_player, &data);
	interfaces::prediction->finish_move(cheat::local_player, user_cmd, &data);
}

void engine_prediction::store(c_user_cmd* user_cmd) {
	cheat::unpredicted_flags = cheat::local_player->get_flags();
}

void engine_prediction::end_prediction(c_user_cmd* user_cmd) {

	interfaces::game_movement->finish_track_prediction_errors(cheat::local_player);
	move_helper->set_host(nullptr);

	cheat::local_player->get_tick_base() = old_tickbase;

	interfaces::global_vars->current_time = engine_prediction::old_curtime;
	interfaces::global_vars->frame_time = engine_prediction::old_frametime;
}

void engine_prediction::create_edgebug_entry(c_user_cmd* user_cmd) {
	// credits: clarity.tk

	features::movement::edgebug_container::prediction::fall_velocity   = cheat::local_player->get_fall_velocity();
	features::movement::edgebug_container::prediction::flags		   = cheat::local_player->get_flags();
	features::movement::edgebug_container::prediction::origin		   = cheat::local_player->get_vec_origin();
	features::movement::edgebug_container::prediction::absolute_origin = cheat::local_player->get_abs_origin();
	features::movement::edgebug_container::prediction::ground_ent	   = cheat::local_player->get_ground_entity();
	interfaces::prediction->setup_move(cheat::local_player, user_cmd, move_helper, &features::movement::edgebug_container::prediction::movement_data);
}

void engine_prediction::apply_edgebug_data(c_user_cmd* user_cmd) {
	// credits: clarity.tk

	interfaces::prediction->finish_move(cheat::local_player, move_helper, &features::movement::edgebug_container::prediction::movement_data);
	features::movement::edgebug_container::prediction::flags |= 2;
	features::movement::edgebug_container::prediction::movement_data.m_buttons |= 4;
	cheat::local_player->get_flags()		 = features::movement::edgebug_container::prediction::flags;
	cheat::local_player->get_fall_velocity() = features::movement::edgebug_container::prediction::fall_velocity;
	cheat::local_player->set_absolute_origin(features::movement::edgebug_container::prediction::absolute_origin);
	cheat::local_player->get_vec_origin()	 = features::movement::edgebug_container::prediction::origin;
	cheat::local_player->get_ground_entity() = features::movement::edgebug_container::prediction::ground_ent;
}

void engine_prediction::apply_edgebug_flags() {
	// credits: clarity.tk

	cheat::local_player->get_flags() = features::movement::edgebug_container::prediction::flags;
}