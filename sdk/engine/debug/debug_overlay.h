#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <chrono>

struct debug_overlay_t {
	bool has_pushed;

	uint64_t calls;
	uint64_t avg_per_sec;

	std::string name;
	std::vector<uint64_t> times;

	debug_overlay_t(std::string name);

	void add(uint64_t time);
	void draw(float pos_x, float pos_y);
};

struct debug_timer_t {
	debug_overlay_t* overlay;
	std::chrono::steady_clock::time_point start;

	debug_timer_t(debug_overlay_t *overlay);
	~debug_timer_t();
};

namespace debug_overlay {

	inline debug_overlay_t* create_move;
	inline debug_overlay_t* do_psfx;
	inline debug_overlay_t* dme;
	inline debug_overlay_t* fsn;
	inline debug_overlay_t* present;
	inline debug_overlay_t* push_notice;
	inline debug_overlay_t* send_datagram;
	inline debug_overlay_t* wucdtb;

	void init();
	void draw();

}
