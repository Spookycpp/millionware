#pragma once

#include "../../source engine/vector.h"

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
	bool line_goes_through_smoke(const vector_t& src, const vector_t& dst);
	bool intersects_hitbox(const vector_t eye_pos, const vector_t end_pos, const vector_t min, const vector_t max, const float radius);

	void set_random_seed(const int seed);

	std::optional< vector_t > get_intersection(const vector_t& start, const vector_t& end, const vector_t& mins, const vector_t& maxs, float radius);
}