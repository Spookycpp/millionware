#include <algorithm>
#include <cstdint>
#include <unordered_map>

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"

#include "../../engine/input/input.h"
#include "../../engine/math/math.h"
#include "../../engine/render/render.h"
#include "../../engine/security/xorstr.h"
#include "../../engine/logging/logging.h"

namespace features::visuals::indicators {

	void indicators() {

		if (!settings.visuals.local.indicators)
			return;

		if (!cheat::local_player || cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
			return;

		if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
			return;

		const auto screen_center = render::get_screen_size() * 0.5f;

		std::vector<std::string> active_binds;

		if (input::is_key_down(settings.miscellaneous.movement.jump_bug_hotkey) && settings.miscellaneous.movement.jump_bug)
			active_binds.push_back(XORSTR("jb"));

		if (input::is_key_down(settings.miscellaneous.movement.edge_bug_hotkey) && settings.miscellaneous.movement.edge_bug)
			active_binds.push_back(XORSTR("eb"));

		if (input::is_key_down(settings.miscellaneous.movement.edge_bug_assist_hotkey) && settings.miscellaneous.movement.edge_bug_assist)
			active_binds.push_back(XORSTR("eba"));

		if (input::is_key_down(settings.miscellaneous.movement.edge_jump_hotkey) && settings.miscellaneous.movement.edge_jump)
			active_binds.push_back(XORSTR("ej"));

		if (input::is_key_down(settings.miscellaneous.movement.fast_walk_hotkey) && settings.miscellaneous.movement.fast_walk)
			active_binds.push_back(XORSTR("fw"));

		// obsecure value
		auto offset = 420.f;

		for (const auto& bind : active_binds) {
			const auto measure = render::measure_text(bind.data(), FONT_VERDANA_24);
			const auto position = screen_center - measure * 0.5f + point_t(0.0f, offset);

			render::draw_text(position, { 255, 255, 255, 220 }, bind.data(), FONT_VERDANA_24);
			offset += measure.y + 8.0f;
		}
	}
}
