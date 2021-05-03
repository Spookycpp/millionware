#pragma once

#include "../../../core/interfaces/interfaces.h"
#include "../../../source engine/input.h"

#include <functional>
#include <deque>

namespace features::engine_prediction {
    struct player_data_t {
        struct lag_velocity_record_t {
            vector_t velocity;
            int tick;
        };

        void update(int ent_idx);

        float simulation_time = 0.0f;
        int last_choke = 0;
        vector_t angles = vector_t();
        vector_t velocity = vector_t();
        vector_t old_velocity = vector_t();
        vector_t position = vector_t();
        vector_t movement = vector_t();
        bool breaking_lc = false;
        bool valid = false;

        std::deque<lag_velocity_record_t> records = {};
    };

    void on_create_move(c_user_cmd *cmd_);
    void on_frame_stage_notify(e_client_frame_stage current_stage);

    // void local_pred( c_user_cmd* );

    void run_command(c_user_cmd *user_cmd);

    int get_predicted_flags();

    bool is_breaking_lc(int ent_idx);

    player_data_t get_player_data(int ent_idx);

    // game movement functions
    vector_t full_walk_move(c_player *, int);
    void check_jump_button(c_player *, vector_t &, vector_t &);
    void start_gravity(c_player *, vector_t &, vector_t &);
    void finish_gravity(c_player *, vector_t &, vector_t &);
    void friction(c_player *, vector_t &);
    void air_move(vector_t &, vector_t &, vector_t &);
    void air_accelerate(vector_t &, const vector_t &, float);
    //void walk_move(c_player *, vector_t &, vector_t &);
    bool categorize_position(c_player *, vector_t &, vector_t &);
    void check_velocity(c_player *, vector_t &, vector_t &);
    void try_player_move(c_player *, vector_t &, vector_t &);

    vector_t aimware_extrapolate(c_player *ent, const vector_t &origin, vector_t &velocity);
    void predict_player(c_player *player);
    int get_predicted_choke(int);
    vector_t extrapolate_player(c_player *, int);
    void trace_player_bbox(c_player *, const vector_t &start, const vector_t &end, trace_t *tr);
    void try_touch_ground(c_player *, const vector_t &start, const vector_t &end, const vector_t &mins, const vector_t &maxs, trace_t *tr);
    void try_touch_ground_in_quadrants(c_player *, const vector_t &start, const vector_t &end, trace_t *tr);
} // namespace engine_prediction