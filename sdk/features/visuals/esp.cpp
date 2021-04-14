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
#include "esp.h"
#include "../../engine/logging/logging.h"

struct entity_esp_t
{
	int predicted_money;
	float last_server_update;
	vector_t position;
};

struct bounding_box_t
{
	float x;
	float y;
	float width;
	float height;
};

static std::unordered_map<int, entity_esp_t> entity_esp_info;

static entity_esp_t &get_entity_info(int index)
{
	if (entity_esp_info.find(index) == entity_esp_info.end())
		entity_esp_info.insert({ index, {} });

	return entity_esp_info[index];
}

static bool get_bounding_box(const entity_esp_t &entity_info, c_entity *entity, bounding_box_t &out_box)
{
	const auto collideable = entity->get_collideable();
	const auto mins = collideable->get_mins();
	const auto maxs = collideable->get_maxs();

	vector_t points[8] =
	{
		{ mins.x, mins.y, mins.z },
		{ mins.x, maxs.y, mins.z },
		{ maxs.x, maxs.y, mins.z },
		{ maxs.x, mins.y, mins.z },
		{ maxs.x, maxs.y, maxs.z },
		{ mins.x, maxs.y, maxs.z },
		{ mins.x, mins.y, maxs.z },
		{ maxs.x, mins.y, maxs.z }
	};

	std::transform(std::begin(points), std::end(points), std::begin(points), [entity] (const auto point)
	{
		return math::vector_transform(point, entity->get_transformation_matrix());
	});

	point_t screen_pos[8];

	for (auto i = 0; i < 8; i++)
	{
		if (!math::world_to_screen(points[i], screen_pos[i]))
			return false;
	}

	auto left = FLT_MAX;
	auto top = FLT_MIN;
	auto right = FLT_MIN;
	auto bottom = FLT_MAX;

	for (auto i = 0; i < 8; i++)
	{
		left = std::min(left, screen_pos[i].x);
		top = std::max(top, screen_pos[i].y);
		right = std::max(right, screen_pos[i].x);
		bottom = std::min(bottom, screen_pos[i].y);
	}

	out_box = { left, bottom, right - left, (top - bottom) + 4.0f };

	return true;
}

void features::visuals::esp::frame()
{
	if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
		return;

	for (auto i = 0; i < interfaces::entity_list->get_highest_ent_index(); i++)
	{
		const auto entity = interfaces::entity_list->get_entity(i);

		if (!entity || !entity->is_player())
			continue;

		auto &entity_info = get_entity_info(i);

		if (!entity->get_networkable()->is_dormant())
		{
			entity_info.last_server_update = interfaces::global_vars->current_time;
			entity_info.position = entity->get_vec_origin();
		}

		const auto player = (c_player *) entity;

		if (player->get_life_state() != LIFE_STATE_ALIVE || player->get_health() <= 0)
			continue;

		const auto time_since_last_update = interfaces::global_vars->current_time - entity_info.last_server_update;
		
		if (time_since_last_update > 3.0f)
			continue;

		bounding_box_t entity_box;

		if (!get_bounding_box(entity_info, entity, entity_box))
			continue;

		const auto box_position = point_t { entity_box.x, entity_box.y };
		const auto box_size = point_t { entity_box.width, entity_box.height };

		render::draw_rect(box_position - 1.0f, box_size + 2.0f, { 0, 0, 0, 100 }, 2.0f);
		render::draw_rect(box_position + 1.0f, box_size - 2.0f, { 0, 0, 0, 100 }, 2.0f);
		render::draw_rect(box_position, box_size, { 255, 255, 255, 200 }, 2.0f);
	}
}

void features::visuals::esp::indicators() {

	//if (!settings.visuals.local.indicators)
	//	return;

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

	auto offset = 420.f;

	for (const auto& bind : active_binds) {
		const auto measure = render::measure_text(bind.data(), FONT_VERDANA_24);
		const auto position = screen_center - measure * 0.5f + point_t(0.0f, offset);

		render::draw_text(position, { 255, 255, 255, 220 }, bind.data(), FONT_VERDANA_24);
		offset += measure.y + 8.0f;
	}
}

void features::visuals::esp::update_dormant_pos(int index, const vector_t &position)
{
	auto &entity_info = get_entity_info(index);

	entity_info.last_server_update = interfaces::global_vars->current_time;
	entity_info.position = position;
}

void features::visuals::esp::update_predicted_money(int index, int money)
{
	auto &entity_info = get_entity_info(index);

	entity_info.predicted_money = money;
}
