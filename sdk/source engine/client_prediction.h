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
    virtual ~c_game_movement(void) {}

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
    DECLARE_VFUNC(3, update(int start_frame, bool is_frame_valid, int inc_ack, int out_cmd), void(__thiscall *)(void *, int, bool, int, int))(start_frame, is_frame_valid, inc_ack, out_cmd);
    DECLARE_VFUNC(12, get_local_view_angles(vector_t &angles), void(__thiscall *)(void *, vector_t &))(angles);
    DECLARE_VFUNC(13, set_local_view_angles(const vector_t &angles), void(__thiscall *)(void *, const vector_t &))(angles);
    DECLARE_VFUNC(18, check_moving_ground(c_entity *entity, double frame_time), void(__thiscall *)(void *, c_entity *, double))(entity, frame_time);
    DECLARE_VFUNC(20, setup_move(c_entity *entity, c_user_cmd *user_cmd, c_move_helper *move_helper, movedata_t *move_data),
                  void(__thiscall *)(void *, c_entity *, c_user_cmd *, c_move_helper *, movedata_t *))
    (entity, user_cmd, move_helper, move_data);
    DECLARE_VFUNC(21, finish_move(c_entity *entity, c_user_cmd *user_cmd, movedata_t *move_data), void(__thiscall *)(void *, c_entity *, c_user_cmd *, movedata_t *))(entity, user_cmd, move_data);

    // values:
    char pad00[8];                // 0x0000
    bool m_bInPrediction;         // 0x0008
    char pad01[1];                // 0x0009
    bool m_bEnginePaused;         // 0x000A
    char pad02[13];               // 0x000B
    bool m_bIsFirstTimePredicted; // 0x0018
};