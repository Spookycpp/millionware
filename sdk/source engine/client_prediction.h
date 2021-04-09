#pragma once

#include "input.h"

class c_move_helper {
private:
	virtual void _vpad() = 0;
public:
	virtual void set_host(class c_entity* host) = 0;
private:
	virtual void unknown1() = 0;
	virtual void unknown2() = 0;
public:
	virtual bool process_impacts() = 0;
};

struct movedata_t {
	char data[184];
};

class c_game_movement {
public:
	virtual	~c_game_movement(void) {}

	virtual void process_movement(void* player, movedata_t* movement) = 0;
	virtual void reset(void) = 0;
	virtual void start_track_prediction_errors(void* player) = 0;
	virtual void finish_track_prediction_errors(void* player) = 0;
	virtual void difference_print(char const* fmt, ...) = 0;
	virtual vector_t const& player_minimums(bool ducked) const = 0;
	virtual vector_t const& player_maximums(bool ducked) const = 0;
	virtual vector_t const& player_view_offset(bool ducked) const = 0;
	virtual bool is_moving_player_stuck(void) const = 0;
	virtual void* get_moving_player(void) const = 0;
	virtual void unblock_pusher(void* player, void* pusher) = 0;
	virtual void setup_movement_bounds(movedata_t* movement) = 0;
};

class c_prediction {
public:
	virtual ~c_prediction(void) = 0;
	virtual void initialize(void) = 0;
	virtual void shutdown(void) = 0;
public:
	virtual void update(int startframe, bool validframe, int incoming_acknowledged, int outgoing_command);
	virtual void pre_entity_packet_recieved(int commands_acknowledged, int current_world_update_packet);
	virtual void post_entity_packet_recieved(void);
	virtual void post_network_data_recieved(int commands_acknowledged);
	virtual void on_recieve_uncompressed_packed(void);
	virtual void get_view_origin(vector_t& org);
	virtual void set_view_origin(vector_t& org);
	virtual void get_view_angles(vector_t& ang);
	virtual void set_view_angles(vector_t& ang);
	virtual void get_local_view_angles(vector_t& ang);
	virtual void set_local_view_angles(vector_t& ang);
	virtual bool in_prediction(void) const;
	virtual bool is_first_time_predicted(void) const;
	virtual int get_last_acknowledged_cmd_number(void) const;
	virtual int get_incoming_packet_number(void) const;
	virtual void check_moving_ground(void* player, double frametime);
	virtual void run_command(void* player, void* user_cmd, c_move_helper* moveHelper);
	virtual void setup_move(void* player, void* user_cmd, c_move_helper* helper, movedata_t* movement);
	virtual void finish_move(void* player, void* user_cmd, movedata_t* movement);
	virtual void set_ideal_pitch(int slot, void* player, const vector_t& origin, const vector_t& angles, const vector_t& viewheight);
	virtual void check_error(int slot, void* player, int commands_acknowledged);
public:
	virtual void _update(int nSlot, bool received_new_world_update, bool validframe, int incoming_acknowledged, int outgoing_command);
	bool perform_prediction(int slot, void* local_player, bool received_new_world_update, int incoming_acknowledged, int outgoing_command);
	void shift_intermediate_data_forward(int slot, int slots_to_remove, int previous_last_slot);
	void restore_entity_to_predicted_frame(int slot, int predicted_frame);
	int first_command_to_execute(int slot, bool received_new_world_update, int incoming_acknowledged, int outgoing_command);
	void dump_entity(void* ent, int commands_acknowledged);
	void shutdown_predictables(void);
	void reinit_predictables(void);
	void remove_stale_predicted_entities(int slot, int last_command_packet);
	void restore_original_ent_state(int slot);
	void run_simulation(int current_command, float curtime, c_user_cmd* user_cmd, void* local_player);
	void untouch(int slot);
	void store_prediction_results(int slot, int predicted_frame);
	bool should_dump_entity(void* ent);
	void smooth_view_on_moving_platform(void* player, vector_t& offset);
	void reset_simulation_tick();
	void show_prediction_list_entry(int list_row, int show_list, void* ent, int& totalsize, int& totalsize_intermediate);
	void finish_prediction_list(int list_row, int show_list, int totalsize, int totalsize_intermediate);
	void check_predict_convar();

	// values:

	char pad0[8];
	bool m_in_prediction;
	char pad1[1];
	bool m_engine_paused;
	char pad2[13];
	bool m_is_first_time_predicted;
};