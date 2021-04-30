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

	void on_frame_stage_notify(e_client_frame_stage frame_stage) {
		switch (frame_stage) {
			case e_client_frame_stage::FRAME_STAGE_RENDER_START: {
				nightmode();
			}
		}
	}

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

	void nightmode() {
		static bool toggled = false;
		static float last_darkness = 0.f;

		auto do_nightmode = settings.visuals.world.nightmode != toggled || last_darkness != settings.visuals.world.nightmode_darkness;

		if (!do_nightmode)
			return;

		toggled = settings.visuals.world.nightmode;
		last_darkness = settings.visuals.world.nightmode_darkness;

		if (!cheat::local_player || !interfaces::engine_client->is_in_game())
			return;

		const static auto draw_specific_static_prop = interfaces::convar_system->find_convar(XORSTR("r_DrawSpecificStaticProp"));

		const float world_darkness = settings.visuals.world.nightmode ? 1.f - (settings.visuals.world.nightmode_darkness / 100.f) : 1.f;
		const float prop_darkness = settings.visuals.world.nightmode ? 1.3f - (settings.visuals.world.nightmode_darkness / 100.f) : 1.f;

		draw_specific_static_prop->set_value(settings.visuals.world.nightmode ? 0 : -1);

		// iterate material handles.
		for (auto handle = interfaces::material_system->first_material(); handle != interfaces::material_system->invalid_material(); handle = interfaces::material_system->next_material(handle)) {

			// get material from handle.
			c_material* material = interfaces::material_system->get_material(handle);
			if (!material)
				continue;

			// if (material->is_error_material())
			//	continue;

			// modulate world materials.
			if (strncmp(material->get_group_name(), XORSTR("World textures"), 14) == 0)
				material->set_color(world_darkness, world_darkness, world_darkness);
			// modulate props materials.
			else if (strncmp(material->get_group_name(), XORSTR("StaticProp textures"), 19) == 0)
				material->set_color(prop_darkness, prop_darkness, prop_darkness);
		}
	}
}
