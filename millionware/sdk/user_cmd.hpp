#pragma once

#include <cstdint>

#include "vector.hpp"

enum e_button_flag {
	BUTTON_IN_ATTACK = 1 << 0,
	BUTTON_IN_JUMP = 1 << 1,
	BUTTON_IN_DUCK = 1 << 2,
	BUTTON_IN_FORWARD = 1 << 3,
	BUTTON_IN_BACK = 1 << 4,
	BUTTON_IN_USE = 1 << 5,
	BUTTON_IN_CANCEL = 1 << 6,
	BUTTON_IN_LEFT = 1 << 7,
	BUTTON_IN_RIGHT = 1 << 8,
	BUTTON_IN_MOVE_LEFT = 1 << 9,
	BUTTON_IN_MOVE_RIGHT = 1 << 10,
	BUTTON_IN_ATTACK2 = 1 << 11,
	BUTTON_IN_RUN = 1 << 12,
	BUTTON_IN_RELOAD = 1 << 13,
	BUTTON_IN_ALT1 = 1 << 14,
	BUTTON_IN_ALT2 = 1 << 15,
	BUTTON_IN_SCORE = 1 << 16,
	BUTTON_IN_SPEED = 1 << 17,
	BUTTON_IN_WALK = 1 << 18,
	BUTTON_IN_ZOOM = 1 << 19,
	BUTTON_IN_WEAPON1 = 1 << 20,
	BUTTON_IN_WEAPON2 = 1 << 21,
	BUTTON_IN_BULLRUSH = 1 << 22,
	BUTTON_IN_GRENADE1 = 1 << 23,
	BUTTON_IN_GRENADE2 = 1 << 24,
	BUTTON_IN_ATTACK3 = 1 << 25,
};

struct user_cmd_t {
private:
	char padding0_[4];

public:
	int command_number;
	int tick_count;
	vector3_t view_angles;
	vector3_t aim_direction;
	float forward_move;
	float side_move;
	float up_move;
	int buttons;

private:
	char padding1_[13];

public:
	short mouse_dx;
	short mouse_dy;
	bool has_been_predicted;
};
