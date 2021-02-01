#pragma once
#include "../../core/interfaces.hpp"
#include "../../core/cheat.hpp"

#include "../../sdk/client_dll.hpp"
#include "../../sdk/user_cmd.hpp"
#include "../../sdk/vector.hpp"
#include "../../sdk/matrix.hpp"

#include <deque>
#include <array>
#include <algorithm>

namespace features::lag_compensation
{
	struct render_record_t {
		std::array< matrix3x4_t, 128 > matrices = {};
		vector3_t origin = vector3_t();
		float simulation_time = 0.0f;
		float global_time = 0.0f;
		float valid_time = 0.0f;
	};

	void on_frame_stage_notify(const int frame_stage);

	void store_visuals();
	bool get_render_record(int idx, matrix3x4_t* out);

	std::deque< render_record_t >& get_raw_render_record(int idx);
}