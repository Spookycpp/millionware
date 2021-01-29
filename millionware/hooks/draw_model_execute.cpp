#include "../core/cheat.hpp"
#include "../core/config.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../utils/xorstr/xorstr.hpp"

void __fastcall hooks::draw_model_execute_hook(uintptr_t ecx, uintptr_t edx, void* ctx, void* state, c_model_render_info* info, matrix3x4_t* matrix) {

	static material_t* textured = nullptr;
	static material_t* flat = nullptr;

	const char* model_name = info->model->name;
	c_entity* entity = interfaces::entity_list->get_by_index(info->entity_index);

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

		if (entity && strstr(model_name, STR_ENC("models/player"))) {

			textured->set_color(255, 255, 255);
			textured->set_flag(MATERIAL_FLAG_IGNORE_Z, false);
			interfaces::model_render->force_material_override(textured);
		}
	}

	hooks::draw_model_execute.get_original<decltype(&draw_model_execute_hook)>()(ecx, edx, ctx, state, info, matrix);
	interfaces::model_render->force_material_override(nullptr);
}