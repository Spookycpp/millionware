#include "../core/cheat.hpp"
#include "../core/config.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../utils/xorstr/xorstr.hpp"

void __fastcall hooks::draw_model_execute_hook(uintptr_t ecx, uintptr_t edx, void* ctx, void* state, c_model_render_info* info, matrix3x4_t* matrix) {

	static material_t* textured = nullptr;
	static material_t* flat = nullptr;

	const char* model_name = info->model->name;
	c_player* entity = (c_player*)interfaces::entity_list->get_by_index(info->entity_index);
	
	// sanity
	if (interfaces::engine_client->is_in_game() && cheat::local_player) {

		// initalize materials
		if (!textured || !flat) {

			textured = interfaces::material_system->find_material(STR_ENC("debug/debugambientcube"));
			flat = interfaces::material_system->find_material(STR_ENC("debug/debugdrawflat"));

			// fixing crashes related to map change
			textured->increment_reference_count();
			flat->increment_reference_count();
		}

		const auto& visible_color = config::get<color_t>(FNV_CT("visuals.enemy.chams.color"));
		const auto& hidden_color = config::get<color_t>(FNV_CT("visuals.enemy.chams_hidden.color"));

		if (entity && entity->is_enemy() && strstr(model_name, STR_ENC("models/player"))) {

			if (config::get<bool>(FNV_CT("visuals.enemy.chams_hidden"))) {
				textured->set_color(hidden_color);
				textured->set_flag(MATERIAL_FLAG_IGNORE_Z, true);
				interfaces::model_render->force_material_override(textured);

				hooks::draw_model_execute.get_original<decltype(&draw_model_execute_hook)>()(ecx, edx, ctx, state, info, matrix);
			}

			if (config::get<bool>(FNV_CT("visuals.enemy.chams"))) {
				textured->set_color(visible_color);
				textured->set_flag(MATERIAL_FLAG_IGNORE_Z, false);
				interfaces::model_render->force_material_override(textured);
			}
		}
	}

	hooks::draw_model_execute.get_original<decltype(&draw_model_execute_hook)>()(ecx, edx, ctx, state, info, matrix);
	interfaces::model_render->force_material_override(nullptr);
}