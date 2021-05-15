#pragma once

#include "../../core/util/util.h"

#include "../../source engine/client_dll.h"
#include "../../source engine/entity.h"
#include "../../source engine/vector.h"

#include <optional>
#include <string>
#include <random>

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

	void force_full_update();
	void set_random_seed(const int seed);
	void set_skybox(const char* name);
	void on_frame_stage_notify(e_client_frame_stage frame_stage);
	void play_sound(const char* file_path, int volume = -1);

	c_player_resource* get_player_resource();

	std::optional< vector_t > get_intersection(const vector_t& start, const vector_t& end, const vector_t& mins, const vector_t& maxs, float radius);
	std::string sanitize_string(const std::string& str);

	template <typename type> type get_random_float_range(const type &min, const type &max) {
        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<type> distribution(min, max);

        return distribution(e2);
    }

	void undo();
}