#pragma once

#include "../../source engine/vector.h"

namespace features::visuals::esp
{
	void frame();

	void update_dormant_pos(int index, const vector_t &position);
	void update_predicted_money(int index, int money);
}
