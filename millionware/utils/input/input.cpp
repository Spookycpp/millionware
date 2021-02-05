#include <map>
#include <windows.h>

#include "input.hpp"



static uintptr_t target_window;
static point_t mouse_pos;

static bool previous_key_state[256];
static bool key_state[256];

static hotkey_state_t empty_hotkey_state = { HOTKEY_PRESSED, 0, false };
static std::map<uint32_t, hotkey_state_t> hotkeys;

void input::update() {
	while (target_window == 0)
		target_window = reinterpret_cast<uintptr_t>(FindWindowA("Valve001", nullptr));

	GetCursorPos(reinterpret_cast<LPPOINT>(&mouse_pos));
	ScreenToClient(reinterpret_cast<HWND>(target_window), reinterpret_cast<LPPOINT>(&mouse_pos));

	memcpy(previous_key_state, key_state, sizeof(key_state));

	for (auto i = 0; i < sizeof key_state; i++) {
		key_state[i] = GetAsyncKeyState(i);
	}

	for (auto &[hash, hotkey] : hotkeys)
	{
		if (hotkey.type == HOTKEY_ALWAYS_ON)
		{
			hotkey.active = true;
		}
		else if (hotkey.type == HOTKEY_NEVER)
		{
			hotkey.active = false;
		}
		else if (hotkey.key != 0 && hotkey.type == HOTKEY_PRESSED)
		{
			hotkey.active = is_key_down(hotkey.key);
		}
		else if (hotkey.key != 0 && hotkey.type == HOTKEY_TOGGLE)
		{
			if (is_key_pressed(hotkey.key))
				hotkey.active ^= true;
		}
		else
		{
			hotkey.active = false;
		}
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

hotkey_state_t &input::get_hotkey(uint32_t hash)
{
	if (hotkeys.find(hash) == hotkeys.end())
		hotkeys.insert({ hash, empty_hotkey_state });

	return hotkeys.at(hash);
}

bool input::is_hotkey_active(uint32_t hash) {
	const auto &hotkey = get_hotkey(hash);

	return hotkey.active;
}

bool input::is_in_bounds(int x1, int y1, int x2, int y2) {
	return mouse_pos.x >= x1 && mouse_pos.y >= y1 && mouse_pos.x <= x2 && mouse_pos.y <= y2;
}

bool input::is_in_bounds(const point_t& min, const point_t& max) {
	return is_in_bounds(min.x, min.y, max.x, max.y);
}
