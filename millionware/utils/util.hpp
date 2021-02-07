#pragma once

#include "../core/interfaces.hpp"
#include "../core/cheat.hpp"
#include "../core/patterns.hpp"

#include <optional>

extern int   TIME_TO_TICKS(float dt);
extern float TICKS_TO_TIME(int tick);
extern float TICK_INTERVAL();

#define FLOW_OUTGOING	0
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out

namespace util {

	float get_total_latency();
	float get_lerp_time();
	float get_random_float(const float min, const float max);
	
	bool run_frame();
	bool line_goes_through_smoke(const vector3_t& src, const vector3_t& dst);
	bool intersects_hitbox(const vector3_t eye_pos, const vector3_t end_pos, const vector3_t min, const vector3_t max, const float radius);

	void set_random_seed(const int seed);
	void set_night_mode();

	std::optional< vector3_t > get_intersection(const vector3_t& start, const vector3_t& end, const vector3_t& mins, const vector3_t& maxs, float radius);

}