#pragma once

#include "input.h"

class c_move_helper {
private:
    virtual void _vpad() = 0;

public:
    virtual void set_host(class c_entity *host) = 0;

private:
    virtual void unknown1() = 0;
    virtual void unknown2() = 0;

public:
    virtual bool process_impacts() = 0;
};

class movedata_t {
public:
    bool m_bFirstRunOfFunctions : 1;
    bool m_bGameCodeMovedPlayer : 1;
    bool m_bNoAirControl : 1;

    unsigned long m_nPlayerHandle;
    int m_nImpulseCommand;
    vector_t m_vecViewAngles;
    vector_t m_vecAbsViewAngles;
    int m_nButtons;
    int m_nOldButtons;
    float m_flForwardMove;
    float m_flSideMove;
    float m_flUpMove;

    float m_flMaxSpeed;
    float m_flClientMaxSpeed;

    vector_t m_vecVelocity;
    vector_t m_vecOldVelocity;
    float somefloat;
    vector_t m_vecAngles;
    vector_t m_vecOldAngles;

    float m_outStepHeight;
    vector_t m_outWishVel;
    vector_t m_outJumpVel;

    vector_t m_vecConstraintCenter;
    float m_flConstraintRadius;
    float m_flConstraintWidth;
    float m_flConstraintSpeedFactor;
    bool m_bConstraintPastRadius;

    void SetAbsOrigin(const vector_t &vec);
    const vector_t &GetAbsOrigin() const;

private:
    vector_t m_vecAbsOrigin;
    char pad_0x160[0x160];
};

class c_game_movement {
public:
    virtual ~c_game_movement(void) {
    }

    virtual void process_movement(void *player, movedata_t *movement) = 0;
    virtual void reset(void) = 0;
    virtual void start_track_prediction_errors(void *player) = 0;
    virtual void finish_track_prediction_errors(void *player) = 0;
    virtual void difference_print(char const *fmt, ...) = 0;
    virtual vector_t const &player_minimums(bool ducked) const = 0;
    virtual vector_t const &player_maximums(bool ducked) const = 0;
    virtual vector_t const &player_view_offset(bool ducked) const = 0;
    virtual bool is_moving_player_stuck(void) const = 0;
    virtual void *get_moving_player(void) const = 0;
    virtual void unblock_pusher(void *player, void *pusher) = 0;
    virtual void setup_movement_bounds(movedata_t *movement) = 0;
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
    virtual void get_view_origin(vector_t &org);
    virtual void set_view_origin(vector_t &org);
    virtual void get_view_angles(vector_t &ang);
    virtual void set_view_angles(vector_t &ang);
    virtual void get_local_view_angles(vector_t &ang);
    virtual void set_local_view_angles(vector_t &ang);
    virtual bool in_prediction(void) const;
    virtual bool is_first_time_predicted(void) const;
    virtual int get_last_acknowledged_cmd_number(void) const;
    virtual int get_incoming_packet_number(void) const;
    virtual void check_moving_ground(void *player, double frametime);
    virtual void run_command(void *player, void *cmd, c_move_helper *moveHelper);
    virtual void setup_move(void *player, void *cmd, c_move_helper *helper, movedata_t *movement);
    virtual void finish_move(void *player, void *cmd, movedata_t *movement);
    virtual void set_ideal_pitch(int slot, void *player, const vector_t &origin, const vector_t &angles, const vector_t &viewheight);
    virtual void check_error(int slot, void *player, int commands_acknowledged);

    // values:
    char pad00[8];                // 0x0000
    bool m_bInPrediction;         // 0x0008
    char pad01[1];                // 0x0009
    bool m_bEnginePaused;         // 0x000A
    char pad02[13];               // 0x000B
    bool m_bIsFirstTimePredicted; // 0x0018
};