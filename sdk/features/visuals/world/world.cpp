#include "world.h"

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

namespace features::visuals::world {

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

	void fog() {
		static const auto fog_override   = interfaces::convar_system->find_convar(XORSTR("fog_override"));
		static const auto fog_start      = interfaces::convar_system->find_convar(XORSTR("fog_start"));
		static const auto fog_end        = interfaces::convar_system->find_convar(XORSTR("fog_end"));
		static const auto fog_maxdensity = interfaces::convar_system->find_convar(XORSTR("fog_maxdensity"));
		static const auto fog_color      = interfaces::convar_system->find_convar(XORSTR("fog_color"));

		const auto& misc_fog_enable = settings.visuals.world.fog;
		const auto& misc_fog_length = settings.visuals.world.fog_length;
		const auto& misc_fog_color  = settings.visuals.world.fog_color;

		static bool    old_enable;
		static int     old_length;
		static color_t old_color;

		//if (misc_fog_enable != old_enable || misc_fog_length != old_length || misc_fog_color != old_color) {
		//	fog_override->set_value(misc_fog_enable);
		//	fog_start->set_value(0);
		//	fog_end->set_value(misc_fog_length);
		//	fog_maxdensity->set_value(misc_fog_color.a / 255.f);
		//	fog_color->set_value(misc_fog_color);
		//	old_enable = misc_fog_enable;
		//	old_length = misc_fog_length;
		//	old_color = misc_fog_color;
		//}
	}
}
