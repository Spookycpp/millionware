#include <Windows.h>

#include "input.hpp"

uintptr_t target_window;
point_t mouse_pos;

bool previous_key_state[256];
bool key_state[256];

void input::update() {
	while (target_window == 0)
		target_window = reinterpret_cast<uintptr_t>(FindWindowA("Valve001", nullptr));

	GetCursorPos(reinterpret_cast<LPPOINT>(&mouse_pos));
	ScreenToClient(reinterpret_cast<HWND>(target_window), reinterpret_cast<LPPOINT>(&mouse_pos));

	memcpy(previous_key_state, key_state, sizeof(key_state));

	for (auto i = 0; i < sizeof key_state; i++) {
		key_state[i] = GetAsyncKeyState(i);
	}
}

point_t input::get_mouse_pos() {
	return mouse_pos;
}

bool input::is_key_pressed(int key) {
	return previous_key_state[key] == false && key_state[key] == true;
}

bool input::is_key_down(int key) {
	return previous_key_state[key] == true && key_state[key] == true;
}

bool input::is_key_released(int key) {
	return previous_key_state[key] == true && key_state[key] == false;
}

bool input::is_key_up(int key) {
	return previous_key_state[key] == false && key_state[key] == false;
}

bool input::is_in_bounds(int x1, int y1, int x2, int y2) {
	return mouse_pos.x >= x1 && mouse_pos.y >= y1 && mouse_pos.x <= x2 && mouse_pos.y <= y2;
}

bool input::is_in_bounds(const point_t& min, const point_t& max) {
	return is_in_bounds(min.x, min.y, max.x, max.y);
}
