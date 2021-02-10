#include "../core/cheat.hpp"
#include "../core/config.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../utils/xorstr/xorstr.hpp"
#include "../utils/util.hpp"

#include "../features/lag compensation/lag_compensation.hpp"

#include <algorithm>

bool draw_all_backtrack_records(c_player* player, material_t* mat, uintptr_t ecx, uintptr_t edx, void* ctx, void* state, c_model_render_info* info, matrix3x4_t* bone_to_world) {
	bool ret = false;

	if (!config::get<bool>(FNV_CT("legitbot.enabled")) || !config::get<bool>(FNV_CT("legitbot.backtrack.enabled")) && !config::get<bool>(FNV_CT("legitbot.triggerbot.backtrack.enabled"))) {
		return false;
	}

	const auto is_time_valid = [&](const float time)
	{
		const c_net_channel_info* nci = interfaces::engine_client->get_net_channel_info();

		if (!nci) {
			return false;
		}

		const float correct = std::clamp(util::get_total_latency(), 0.0f, 0.2f);
		const float delta = correct - (static_cast<float>(cheat::local_player->tick_base()) * interfaces::global_vars->interval_per_tick - time);

		if (config::get<int>(FNV_CT("misc.fake_ping.enabled"))) {
			return std::abs(delta) < 0.150f;
		}

		const float backtrack_time = config::get<bool>(FNV_CT("legitbot.backtrack.enabled")) ?
			static_cast<float>(config::get<bool>(FNV_CT("legitbot.backtrack.time"))) :
			static_cast<float>(config::get<bool>(FNV_CT("legitbot.triggerbot.backtrack.time")));

		return std::abs(delta) <= backtrack_time * 0.001f;
	};

	auto& data = features::lag_compensation::get_raw_render_record(player->networkable()->index());

	for (auto it = data.rbegin(); it != data.rend(); ++it)
	{
		if (!is_time_valid(it->simulation_time)) {
			continue;
		}

		if (it->origin.dist(player->origin()) < 1.0f) {
			continue;
		}

		if (!it->matrices.data()) {
			continue;
		}

		ret = true;

		if (!config::get<bool>(FNV_CT("visuals.enemy.chams")))
		{
			mat->set_flag(MATERIAL_FLAG_IGNORE_Z, true);

			float col[3] = { 1.0f, 1.0f, 1.0f };
			interfaces::engine_render->set_blend(0.05f);
			interfaces::engine_render->set_color_modulation(reinterpret_cast<float*>(&col));

			interfaces::model_render->force_material_override(mat);

			hooks::draw_model_execute.get_original<decltype(&hooks::draw_model_execute_hook)>()(ecx, edx, ctx, state, info, it->matrices.data());
		}

		mat->set_flag(MATERIAL_FLAG_IGNORE_Z, false);

		float col[3] = { 1.0f, 1.0f, 1.0f };
		interfaces::engine_render->set_blend(0.05f);
		interfaces::engine_render->set_color_modulation(reinterpret_cast<float*>(&col));

		interfaces::model_render->force_material_override(mat);

		hooks::draw_model_execute.get_original<decltype(&hooks::draw_model_execute_hook)>()(ecx, edx, ctx, state, info, it->matrices.data());
	}

	return ret;
}

bool draw_last_backtrack_record(c_player* player, material_t* mat, uintptr_t ecx, uintptr_t edx, void* ctx, void* state, c_model_render_info* info, matrix3x4_t* matrix)
{
	if (!config::get<bool>(FNV_CT("legitbot.enabled")) || !config::get<bool>(FNV_CT("legitbot.backtrack.enabled")) && !config::get<bool>(FNV_CT("legitbot.triggerbot.backtrack.enabled"))) {
		return false;
	}

	std::array< matrix3x4_t, 128 > matrices = {};

	if (!features::lag_compensation::get_render_record(info->entity_index, matrices.data())) {
		return false;
	}

	if (!matrices.data()) {
		return false;
	}

	if (!config::get<bool>(FNV_CT("visuals.enemy.chams")))
	{
		mat->set_flag(MATERIAL_FLAG_IGNORE_Z, true);

		float col[3] = { 1.0f, 1.0f, 1.0f };
		interfaces::engine_render->set_blend(0.05f);
		interfaces::engine_render->set_color_modulation(reinterpret_cast<float*>(&col));

		interfaces::model_render->force_material_override(mat);

		hooks::draw_model_execute.get_original<decltype(&hooks::draw_model_execute_hook)>()(ecx, edx, ctx, state, info, matrices.data());
	}

	mat->set_flag(MATERIAL_FLAG_IGNORE_Z, false);

	float col[3] = { 1.0f, 1.0f, 1.0f };
	interfaces::engine_render->set_blend(0.05f);
	interfaces::engine_render->set_color_modulation(reinterpret_cast<float*>(&col));

	interfaces::model_render->force_material_override(mat);

	hooks::draw_model_execute.get_original<decltype(&hooks::draw_model_execute_hook)>()(ecx, edx, ctx, state, info, matrices.data());

	return true;
}

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
		const auto& hidden_color  = config::get<color_t>(FNV_CT("visuals.enemy.chams_hidden.color"));
		const auto& history_color = config::get<color_t>(FNV_CT("visuals.enemy.chams.records.color"));

		if (strstr(info->model->name, STR_ENC("shadow")) != nullptr) {
			return;
		}

		if (entity && entity->is_enemy() && strstr(model_name, STR_ENC("models/player"))) {

			if (config::get<bool>(FNV_CT("visuals.enemy.chams_hidden"))) {
				textured->set_color(hidden_color);
				textured->set_flag(MATERIAL_FLAG_IGNORE_Z, true);
				interfaces::model_render->force_material_override(textured);

				hooks::draw_model_execute.get_original<decltype(&draw_model_execute_hook)>()(ecx, edx, ctx, state, info, matrix);
			}

			auto player = (c_entity*)interfaces::entity_list->get_by_index(info->entity_index);

			if (config::get<bool>(FNV_CT("visuals.enemy.chams.records"))) {
				draw_last_backtrack_record((c_player*)player, textured, ecx, edx, ctx, state, info, matrix);
				draw_all_backtrack_records((c_player*)player, textured, ecx, edx, ctx, state, info, matrix);
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